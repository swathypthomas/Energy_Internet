//gcc fwd_req.c pub_sub.c -o fwd_req -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent

#include "fwd_req.h"

int main (int argc, char **argv)  
{
	fwdReq();
	return 0;
}

//REQn MessageId frm_IP Power Duration to_IP through_IP Port 
void forwardReqToPeers(char *index, char *reqId, char *frmIp, char *power, char *duration, char *throughIp, char *ports)
{
	redisReply *r1;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
		// handle error
	}
	r1 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
	char message[500] = "REQ";
	strcat(message, index);
	strcat(message, " ");
	strcat(message, reqId);     
	strcat(message, " ");
	strcat(message, frmIp);
	strcat(message, " ");
	strcat(message, power);
	strcat(message, " ");
	strcat(message, duration);
	strcat(message, " ");
	char messageSent[500];
	if (r1->type == REDIS_REPLY_ARRAY) 
	{
        for (int j = 0; j < r1->elements; j+=2) 
        {
        	strcpy(messageSent, message);
        	char *peer_ip = r1->element[j]->str;
        	char *peer_port = r1->element[j+1]->str;
			strcat(messageSent, peer_ip);
			strcat(messageSent, " ");
			strcat(messageSent, throughIp);
			strcat(message, " ");
			strcat(message, ports);
			strcat(message, " ");
        	strcat(messageSent, peer_port);
        	pub_sub("pub", peer_ip, messageSent, NULL, NULL);
        }
    }
}

void fwdReq()
{	
	redisReply *reply, *rf, *r5;
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
			// handle error
	}
	while(1)
	{
		reply = redisCommand(k, "BLPOP %s %d", "FWD_BOARD", 0);
		redisCommand(k,"DECRBY %s %d", "NEW_FWD", 1);
		char *r = reply->element[1]->str;
		char *token = strtok(r, " ");
		int i=0;
		char splitMessage[24][16];         // max 24 fields each of max size 16
											// loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], " %s\n", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
	//REQn MessageId frm_IP Power Duration to_IP through_IP Port 
		if(wildCardCmp("REQ*",splitMessage[0]))
		{
			char *str = splitMessage[0], *p = str;
			int n = intFrmStr(p);
			n--;
			rf = redisCommand(k,"GET %s", "PUB_SUB_IP");
			char *proxyIp = rf->str;
			int j, l=7;
			char ports[100];
			for(j=0; j<=(i-8); j++)
			{
				if(j == 0)
				{
					strcpy(ports, splitMessage[l]);
					strcat(ports, " ");
					l++;
				}
				else
				{
					strcat(ports, splitMessage[l]);
					strcat(ports, " ");
					l++;						
				}
			}
			char frmIp[16];
			if(strcmp(splitMessage[6], "0")==0)
			{
				strcpy(frmIp, splitMessage[2]);
			}
			else
			{
				strcpy(frmIp, splitMessage[6]);
			}
			char frmPort[3] = "";
			r5 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
			if (r5->type == REDIS_REPLY_ARRAY) 
			{
				for (int d = 0; d < r5->elements; d+=2) 
				{
					char *peer_ip = r5->element[d]->str;
					char *peer_port = r5->element[d+1]->str;
					if(strcmp(peer_ip, frmIp)==0)
					{
						strcpy(frmPort, peer_port);
					}
				}
			}
			strcat(ports, frmPort);
			strcat(ports, " ");
			char inx[3];
			sprintf(inx, "%d", n);
			forwardReqToPeers(inx, splitMessage[1], splitMessage[2], splitMessage[3], splitMessage[4], proxyIp, ports);
		}
	}
}


bool wildCardCmp(char *pattern, char *string)
{
	if(*pattern=='\0' && *string=='\0')		// Check if string is at end or not.
		return true;
		
	if(*pattern=='?' || *pattern==*string)		//Check for single character missing or match
		return wildCardCmp(pattern+1,string+1);
		
	if(*pattern=='*')
		return wildCardCmp(pattern+1,string) || wildCardCmp(pattern,string+1);		// Check for multiple character missing
		
	return false;
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