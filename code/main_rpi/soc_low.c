//gcc soc_low.c pub_sub.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/soc_low -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis -levent

#include "soc_low.h"


int main (int argc, char **argv)  
{
	socLow();
	return 0;
}

void publishReqToPeers(int index, char *power, char *duration, char *rn, char *toIp)
{
	//printf("Crafting request\n");		//trace
	redisReply *r1, *r3, *r4, *log;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
		// handle error
	}
	//REQn MessageId frm_IP Power Duration to_IP through_IP Port 
	r3 = redisCommand(k, "GET %s", "PUB_SUB_IP");
	r4 = redisCommand(k, "GET %s", "ID");
	char message[500];
	strcpy(message,"REQ");
	char n[3];
	sprintf(n, "%d", index);
	strcat(message, n);                       //message = REQ0
	char r[4];
	strcpy(r, rn); 							  //r = random number as string
	char reqId[9]; 
	strcpy(reqId, r4->str); 
	strcat(reqId, ":");                       //reqId=ER1: and message=REQ0
	strncat(reqId, r, 4);
						//printf("\n\n swathy reqId=%s message=%s swathy\n\n", reqId, message);
	strcat(message, " ");
	strcat(message, reqId);  
	redisCommand(k, "SET %s %d", reqId, 1);
	strcat(message, " ");           
	log = redisCommand(k, "SET %s %d", reqId, 1);
	r4 = redisCommand(k, "GET %s", "TIMESLOT");
	int eT = intFrmStr(r4->str);
	redisCommand(k, "EXPIRE %s %d", reqId, eT);      //keep messageID for 3minutes
	strcat(message, r3->str);
	strcat(message, " ");
	strcat(message, power);
	strcat(message, " ");
	strcat(message, duration);
	strcat(message, " ");
	char messageSent[500];
	char *peer_port;
	if(strcmp(toIp, "0") == 0)
	{
		r1 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
		if (r1->type == REDIS_REPLY_ARRAY) 
		{
      	  for (int j = 0; j < r1->elements; j+=2) 
      	  	{
	        	strcpy(messageSent, message);
	        	char *peer_ip = r1->element[j]->str;
	        	peer_port = r1->element[j+1]->str;
	        	strcat(messageSent, peer_ip);
	        	strcat(messageSent, " ");
	        	strcat(messageSent, "0");       //through_IP=0 when message initially sent
	        	strcat(messageSent, " ");
	        	strcat(messageSent, peer_port);
	        	//printf("\n%s %s\n", peer_ip, messageSent);
	        	pub_sub("pub", peer_ip, messageSent, "0", "0");
    	   	}
   		}
	}
	else
	{
		r1 = redisCommand(k, "HGET %s %s", "PEER:IP:PORT", toIp);
		peer_port = r1->str;
		strcpy(messageSent, message);
		strcat(messageSent, toIp);
	    strcat(messageSent, " ");
	    strcat(messageSent, "0");       //through_IP=0 when message initially sent
	    strcat(messageSent, " ");
	    strcat(messageSent, peer_port);
	        	//printf("\n%s %s\n", peer_ip, messageSent);
	    pub_sub("pub", toIp, messageSent, "0", "0");
	}
    freeReplyObject(r4);
	redisFree(k);
	freeReplyObject(r1);
	freeReplyObject(r3);
}

void retransmitReq(int index, char *power, char *duration, char *rn)
{
	int ackCount, m;
	char splitMessage[24][16];
	redisReply *reply, *r1;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
		// handle error
	}
	r1 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
	int noOfPeers = (r1->elements)/2;
	char ip[noOfPeers][16];
	if (r1->type == REDIS_REPLY_ARRAY) 
	{
        for (int j = 0, l = 0; j < r1->elements; j+=2, l+=1) 
        {
        	strcpy(ip[l], r1->element[j]->str);
        }
    }
	reply = redisCommand(k, "GET %s", "NEW_ACKREQ");
	ackCount = reply->integer;
	if(ackCount >= noOfPeers)
		;
	else if(ackCount == 0)
	{
		publishReqToPeers(index, power, duration, rn, "0");	
	}
	else
	{
		for(int n = 0; n < ackCount; n++)
		{
			reply = redisCommand(k, "LPOP %s", "ACK_REQ_BOARD");
			char *t = strtok(reply->str, "\n");
			char *token = strtok(t, " ");
			int i=0;
				   // loop through the string to extract all other tokens
			while( token != NULL ) 
			{
			    sprintf(splitMessage[i], "%s", token ); //printing each token
			    token = strtok(NULL, " ");
			    i++;
			}
			for(m = 0; m < noOfPeers; m++)
			{
				if(strcmp(ip[m], splitMessage[2]) == 0)
				{
					strcpy(ip[m], "0");
				}

			}
		}
		for(m = 0; m < noOfPeers; m++)
		{
			if(strcmp(ip[m], "0") != 0)
			{
				publishReqToPeers(index, power, duration, rn, ip[m]);
			}
		}
	}
}

void socLow()   //monitor for lower threshold of SOC
{
	redisReply *reply, *q, *s, *x, *u, *y, *b, *r5, *r6, *rf;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	char ports[100] = "";
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
			// handle error
	}
	int index = 0;
	while(1)
	{
		if(index == 0)
		{
			redisCommand(k, "BLPOP %s %d", "LOWER_THRESHOLD", 0);			
		}
		reply = redisCommand(k, "LPOP %s", "PEER_SELECTED");
		if(reply->str)
		{
			redisCommand(k, "LPUSH %s, %s", "PEER_SELECTED", "1");
			reply = redisCommand(k, "GET %s", "TIMESLOT");
			sleep(intFrmStr(reply->str));                                             //if power transfer is in process, wait for the minimum interval time of 5min that can happen. This saves CPU cycles
		}
		else
		{
			char rN[4];
			int r = rnGn();
			sprintf(rN,"%d",r);
			for(index=0; index<10; index++)     //goes 10 levels deep
			{				
				reply = redisCommand(k, "GET %s", "POWER_REQUEST");
				char *power = reply->str;
				reply = redisCommand(k, "GET %s", "TIMESLOT");
				char *duration = reply->str;       //15 Seconds
				redisCommand(k, "SET %s %d", "NEW_AVL", 0);
				redisCommand(k, "DEL %s", "AVL_BOARD");
				redisCommand(k, "DEL %s", "ACK_REQ_BOARD");
				redisCommand(k, "SET %s %d", "NEW_ACKREQ", 0);
				publishReqToPeers(index, power, duration, rN, "0"); 
				sleep(5);                           //REQUEST FOR ENERGY
			//	retransmitReq(index, power, duration, rN);
			//	sleep(3);																  //WAIT ONE SECOND
				reply = redisCommand(k, "GET %s", "NEW_AVL");							  //CHECK ANY AVL MESSAGE RECEIVED
				if(strcmp(reply->str, "0") == 0)
					continue;
				char splitMessage[24][16];         // max 24 fields each of max size 16
				reply = redisCommand(k, "DEL %s", "PORT_LIST");
				reply = redisCommand(k, "DEL %s", "DURATION_LIST");
				reply = redisCommand(k, "DEL %s", "POWER_LIST");
				reply = redisCommand(k, "DEL %s", "FROM_IP_LIST");
				reply = redisCommand(k, "DEL %s", "TARRIF_LIST");
				reply = redisCommand(k, "DEL %s", "ACK_ID");
				reply = redisCommand(k, "LPOP %s", "AVL_BOARD");
				while(reply->str)
				{
					redisCommand(k,"DECRBY %s %d", "NEW_AVL", 1);                
					char *token = strtok(reply->str, " ");
					int i=0, j=0, l=6;
				   // loop through the string to extract all other tokens
				   while( token != NULL ) 
				   {
				      sprintf(splitMessage[i], "%s", token ); //printing each token
				      token = strtok(NULL, " ");
				      i++;
				   }
        		//AVL messageId TARRIF DURATION POWER FROM_IP PORTS
					
					for(; j<(i-6); j++)
					{
						if(j==0)
						{
							strcpy(ports, splitMessage[l]);
							l++;
						}
						else
						{
							strcat(ports, " ");
							strcat(ports, splitMessage[l]);
							l++;								
						}
					}
					redisCommand(k, "ZADD %s %s %s", "DURATION_LIST", splitMessage[2], splitMessage[3]);
					redisCommand(k, "ZADD %s %s %s", "POWER_LIST", splitMessage[2], splitMessage[4]);
					redisCommand(k, "ZADD %s %s %s", "FROM_IP_LIST", splitMessage[2], splitMessage[5]);
					redisCommand(k, "ZADD %s %s %s", "PORT_LIST", splitMessage[2], ports);
					redisCommand(k, "ZADD %s %s %s", "ACK_ID", splitMessage[2], splitMessage[1]);
					redisCommand(k, "ZADD %s %s %s", "TARRIF_LIST", splitMessage[2], splitMessage[2]);
					//printf("Ports=%sEND\n", ports);															//TRACE
					reply = redisCommand(k, "LPOP %s", "AVL_BOARD");
				}
				q = redisCommand(k, "ZPOPMIN %s", "FROM_IP_LIST");
				s = redisCommand(k, "ZPOPMIN %s", "POWER_LIST");
				u = redisCommand(k, "ZPOPMIN %s", "DURATION_LIST");
				x = redisCommand(k, "ZPOPMIN %s", "PORT_LIST");
				y = redisCommand(k, "ZPOPMIN %s", "ACK_ID");
				b = redisCommand(k, "ZPOPMIN %s", "TARRIF_LIST");
				int w = 0;
	          	redisCommand(k, "SET %s %s", "FROM_IP", q->element[w]->str);
	          	redisCommand(k, "SET %s %s", "POWER", s->element[w]->str);
	          	redisCommand(k, "SET %s %s", "TIME", u->element[w]->str);
	          	char *pP = x->element[w]->str;
	          	pP[strcspn(pP, "\n")] = '\0';
	          	redisCommand(k, "SET %s %s", "PORT_PATH", pP);
	          	redisCommand(k, "SET %s %s", "TRANSACTION_ID", y->element[w]->str);
	          	redisCommand(k, "SET %s %s", "COST", b->element[w]->str);
	          	redisCommand(k, "LPUSH %s %s", "PEER_SELECTED", "1");                  //flag to indicate that a peer with lowest tarrif is selected
				index = 0;
				break;
			}
		}
	}
}


int rnGn() 
{
    srand(time(NULL));
    int n = rand()%1000 + 1000;
    int m = rand()%1000 + 1000;
    int l = rand()%1000 + 1000;
    int k = (m+n+l)/3;
    return k;
}

int intFrmStr(char *p)
{
	int val;
	while (*p) 
	{ // While there are more characters to process...
	    if ( isdigit(*p) || ( (*p=='-'||*p=='+') && isdigit(*(p+1)) )) 
	    {
	        // Found a number
	        val = strtol(p, &p, 10); // Read number
	    } else 
	    {
	        // Otherwise, move on to the next character.
	        p++;
	    }
	}
	return val;
}