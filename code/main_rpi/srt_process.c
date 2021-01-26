//gcc srt_process.c pub_sub.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/srt_process -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis -levent
//This controls the response to start and stop power transfer messages by adding or subtracting power to ports
#include "transfer_process.h"


int main (int argc, char **argv)  
{
	srtProcess();
	return 0;
}

//STP MessageId Power frm_IP to_IP Port_list
void srtProcess()
{
	redisReply *reply, *r1, *r2;	
	redisContext *k = redisConnect("127.0.0.1", 6379);
	char message[500];
	float cl;
	r1 = redisCommand(k, "GET %s", "PUB_SUB_IP");
	int i=0, j=0, m;
	float p, pA, pB;
	char selectedIpInPath[16];
	char splitMessage[24][16];
	while(1)
	{	
		reply = redisCommand(k, "BLPOP %s %d", "SRT_BOARD", 0);
		redisCommand(k,"DECRBY %s %d", "NEW_SRT", 1);
		char srtCommand[100] = "";
		strcat(srtCommand, reply->element[1]->str);
		char savedCommand[100] = "";
		strcat(savedCommand, srtCommand);
		char *t = strtok(srtCommand, "\n");
		char *token = strtok(t, " ");						   // loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
		j = i;
		j = j-1;      //printf("comparison.%s.%s.%s\n", r1->str, splitMessage[4], splitMessage[3]);
		if(strcmp(r1->str, splitMessage[4]) == 0)											//SOURCE
		{				
			p = strtof(splitMessage[2], NULL); 
			reply = redisCommand(k, "GET %s", splitMessage[j]); //printf("reply->str = %s\n", reply->str);
			pA =  strtof(reply->str, NULL); //printf("pA=%f\np=%f", pA, p);
			pA = pA - p;																	//PORT POWER(+ if in and - if out)
			redisCommand(k, "SET %s %.2f", splitMessage[j], pA);  printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
			redisCommand(k, "SET %s %s", "REFRESH", splitMessage[j]);
			redisCommand(k, "DEL %s", "REFRESH1");
			r2 = redisCommand(k, "GET %s", "CRITICAL_LOAD");			//update critical load value
			cl = strtof(r2->str, NULL);       //printf("p=%f cl_initial=%f\n",p, cl);
			cl = cl + p;                 printf("New value of CRITICAL LOAD=%.2f\n", cl);
			r2 = redisCommand(k, "SET %s %.2f", "CRITICAL_LOAD", cl);
			r2 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
			if (r2->type == REDIS_REPLY_ARRAY) 
			{
				for (int f = 0; f < r2->elements; f+=2) 
				{
					char *peer_ip = r2->element[f]->str;
					char *peer_port = r2->element[f+1]->str;
					if(strcmp(peer_port, splitMessage[j]) == 0)
					{
						strcpy(selectedIpInPath, peer_ip);
						break;
					}
				}
			}
			for(m=0; m<j; m++)
			{
				if(m==0)
				{
					strcpy(message, splitMessage[m]);			    		
				}
				else
				{
					strcat(message, " ");
					strcat(message, splitMessage[m]);			    		
				}
			}
			printf("selectedIpInPath=%s\n", selectedIpInPath);
			pub_sub("pub", selectedIpInPath, message, "0", "0");
			redisCommand(k, "RPUSH %s %s", "TRACK_POWER_OUT", message);
		}
		else if(strcmp(r1->str, splitMessage[3]) == 0)										//SINK
		{		//printf("destination router\n");	
			p = strtof(splitMessage[2], NULL);
			reply = redisCommand(k, "GET %s", splitMessage[j]);
			pA =  strtof(reply->str, NULL);
			pA = pA + p;																	//PORT POWER(+ if in and - if out)
			redisCommand(k, "SET %s %.2f", splitMessage[j], pA);printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
			redisCommand(k, "SET %s %s", "REFRESH", splitMessage[j]);
			redisCommand(k, "DEL %s", "REFRESH1");
		}
		else																				//INTERMEDIATE
		{			
			p = strtof(splitMessage[2], NULL);
			reply = redisCommand(k, "GET %s", splitMessage[j]);
			pA = strtof(reply->str, NULL);
			r2 = redisCommand(k, "GET %s", splitMessage[j-1]);
			pB = strtof(r2->str, NULL);
			pA = pA + p;																	//PORT POWER(+ if in and - if out)
			pB = pB - p;
			redisCommand(k, "SET %s %.2f", splitMessage[j], pA);printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
			redisCommand(k, "SET %s %.2f", splitMessage[j-1], pB);printf("Power through port %s is %.2fW\n", splitMessage[j-1], pB);
			redisCommand(k, "SET %s", "REFRESH", splitMessage[j]);
			redisCommand(k, "SET %s", "REFRESH1", splitMessage[j-1]);
			r2 = redisCommand(k, "HGETALL %s", "PEER:IP:PORT");
			if (r2->type == REDIS_REPLY_ARRAY) 
			{
				for (int f = 0; f < r2->elements; f+=2) 
				{
					char *peer_ip = r2->element[f]->str;
					char *peer_port = r2->element[f+1]->str;
					if(strcmp(peer_port, splitMessage[j-1]) == 0)
					{
						strcpy(selectedIpInPath, peer_ip);
					}
				}
			}
			for(m=0; m<(j-1); m++)
			{
				if(m==0)
				{
					strcpy(message, splitMessage[m]);   		
				}
				else
				{
					strcat(message, " ");			 
					strcat(message, splitMessage[m]);   		
				}
			}
			pub_sub("pub", selectedIpInPath, message, "0", "0");
		}
	}	
	freeReplyObject(reply);	
	freeReplyObject(r1);	
	freeReplyObject(r2);
	redisFree(k);
}
