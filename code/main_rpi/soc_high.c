//gcc soc_high.c pub_sub.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/soc_high -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis -levent

#include "soc_high.h"

char rN[4];


int main (int argc, char **argv)  
{
	socHigh();
	return 0;
}

//AVAILABLE Message format is "AVL messageId 30.9(TARRIF IN RUPEES) 5(DURATION IN minutes) 3(POWER in watts) IP(FROM IP) PORT1 PORT2 PORT3(CHAIN OF PORTS FOR IN OUT SEQUENCE OF POWER)"
void publishAvlToPeers(char *tarrif, char *t, char *pReq, char *myIp, char *toIP, char *ports, char *reqId)
{
	char message[500] = "";
	strcat(message, "AVL");
	strcat(message, " ");
	strcat(message, reqId);
	strcat(message, " ");
	strcat(message, tarrif);
	strcat(message, " ");
	strcat(message, t);
	strcat(message, " ");
	strcat(message, pReq);
	strcat(message, " ");
	strcat(message, myIp);
	strcat(message, " ");
	strcat(message, ports);
	pub_sub("pub", toIP, message, "0", "0");
}

void publishNavlToPeers(char *messageId, char *frm_IP, char *toIP)
{
	char message[32] = "";
	strcat(message, "NAVL");
	strcat(message, " ");
	strcat(message, messageId);
	strcat(message, " ");
	strcat(message, frm_IP);
	pub_sub("pub", toIP, message, "0", "0");
}

void socHigh()
{
	redisReply *reply, *r5, *r6, *reply1, *req;
	int notAvailableFlag;
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
			// handle error
	}
	while(1)
	{
		req = redisCommand(k, "BLPOP %s %d", "REQ_BOARD", 0);
		redisCommand(k,"DECRBY %s %d", "NEW_REQ", 1);
		char *r = req->element[1]->str;
		char *t = strtok(r, "\n");
		char *token = strtok(t, " ");
		int i=0;
		char splitMessage[24][16];         // max 24 fields each of max size 16
											   // loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
		float soc;
		reply1 = redisCommand(k, "GET %s", "SOC_A");
		reply = redisCommand(k, "GET %s", "SOC");
		float soc_v = strtof(reply->str, NULL);
		if(!(reply1->str));
		else
		{
			float soc_a = strtof(reply1->str, NULL);
			if(soc_a < soc_v)
			{
				soc_v = soc_a;
			}
		}
		notAvailableFlag = 0;
		if (soc_v > 70)			//Available to source energy
		{
			float excessSoc = soc_v - 70.0; 
			reply1 = redisCommand(k, "GET %s", "CAPACITY");
			float capacity = strtof(reply1->str, NULL);
			float excessWs = (excessSoc/100) * capacity;     //wattseconds
			//REQn MessageId frm_IP Power Duration to_IP through_IP Port
			float pReq = strtof(splitMessage[3], NULL);
			reply1 = redisCommand(k, "GET %s", "CRITICAL_LOAD");
			float cl = strtof(reply1->str, NULL);
			float t = excessWs/(pReq+cl);
			t = t*.023;      //scaled to make 80%SOC as capable of providing critical load+20watt for 20seconds
			reply = redisCommand(k, "GET %s", splitMessage[1]);
			int f = 0;
			if(!(reply->str))
			{
				f = 1;
			}
			reply = redisCommand(k, "GET %s", "TIMESLOT");
			float tS = strtof(reply->str, NULL);
			if((t>tS) && (f != 0))	//Discard message if the messageId has been received previously									//if the ER is capable to supply for more than 5min
			{
				notAvailableFlag = 1;
				reply = redisCommand(k, "GET %s", "TARRIF");
				float tarrifStart = strtof(reply->str, NULL);
				float tReq = strtof(splitMessage[4], NULL);
				if(t>tReq)
				{
					t = tReq;							//supply for a maximum of requested time duration or above the min time threshold set
				}
				float socEnd;
				socEnd = soc_v-((pReq*t*100)/capacity);
				redisCommand(k, "SET %s %.2f", "SOC_A", socEnd);
				float tarrifEnd = 50 + 20*(1-socEnd/100);    //tarrif after transfer of power, based on soc. tarrif = x + y(1-soc)
				float tarrif  = (tarrifStart + tarrifEnd)/2;   //average tarrif for the transaction
				reply = redisCommand(k,"GET %s", "PUB_SUB_IP");
				char *myIp = reply->str;
				char *toIP = splitMessage[2];
				char ports[100] = "";
				int j=0, l=7, m=7;
				for(; j<(i-m); j++)
				{
					strcat(ports, splitMessage[l]);
					strcat(ports, " ");
					l++;
				}
				printf("Port list=%s\n", ports);
				int position = 6;             //position of ip address in message
				if(strcmp(splitMessage[6], "0")==0)
				{
					position = 2;
				}
				char *throughIp = splitMessage[position];
				char throughPort[3] = "";
				r5 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
				if (r5->type == REDIS_REPLY_ARRAY) 
				{
        			for (int g = 0; g < r5->elements; g++) 
        			{
        				char *peer_ip = r5->element[g]->str;
        				g++;
        				char *peer_port = r5->element[g]->str;
        				if(strcmp(peer_ip, throughIp)==0)
        				{
        					strcpy(throughPort, peer_port);
        				}
        			}
        		}
        		strcat(ports, throughPort);
				printf("Port list=%s\n", ports);
        		char tarrifStr[5];
        		gcvt(tarrif, 4, tarrifStr);
        		char t1[5];
        		gcvt(t, 4, t1);
        		char pReqStr[5];
        		gcvt(pReq, 4, pReqStr);
        		//AVL messageId TARRIF DURATION POWER FROM_IP PORTS
        		publishAvlToPeers(tarrifStr, t1, pReqStr, myIp, toIP, ports, splitMessage[1]);
        	}
		}
		if(notAvailableFlag == 0)
		{
			publishNavlToPeers(splitMessage[1], splitMessage[5], splitMessage[2]);
		}
	}
}