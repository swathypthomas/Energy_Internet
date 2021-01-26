//gcc stp_process.c pub_sub.c -o stp_process -lhiredis -levent
//This controls the response to start and stop power transfer messages by adding or subtracting power to ports
#include "transfer_process.h"


int main (int argc, char **argv)  
{
	stpProcess();
	return 0;
}

//STP MessageId Power frm_IP to_IP Port_list
void stpProcess()
{
	redisReply *reply, *r1, *r2, *r3, *r4, *r5, *r6;	
	redisContext *k = redisConnect("127.0.0.1", 6379);
	char message[500];
	float cl;
	r1 = redisCommand(k, "GET %s", "PUB_SUB_IP");
	int i=0, j=0, m;
	float p, pA, pB;
	char selectedIpInPath[16];
	char splitMessage[24][16];
		// max 24 fields each of max size 16
	while(1)
	{
		reply = redisCommand(k, "BLPOP %s %d", "STP_BOARD", 0);
		char *stpCommand = reply->element[1]->str;
		redisCommand(k,"DECRBY %s %d", "NEW_STP", 1);
		char *t = strtok(stpCommand, "\n");
		char *token = strtok(t, " ");
											// loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
		j=i;
		j = j-1;
		char stpReceived[13] = "";
		strcat(stpReceived, "STP");
		strcat(stpReceived, splitMessage[1]);
		r3 =redisCommand(k, "GET %s", stpReceived);
		if(!(r3->str))
		{
			redisCommand(k, "SET %s %d", stpReceived, 1);
			redisCommand(k, "EXPIRE %s %d", stpReceived, 30);
			if(strcmp(r1->str, splitMessage[4]) == 0)											//SOURCE
			{				
				p = strtof(splitMessage[2], NULL);
				reply = redisCommand(k, "GET %s", splitMessage[j]);
				pA =  strtof(reply->str, NULL);
				pA = pA + p;																	//PORT POWER(+ if in and - if out)
				redisCommand(k, "SET %s %.2f", splitMessage[j], pA);printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
				redisCommand(k, "SET %s %s", "REFRESH", splitMessage[j]);
				redisCommand(k, "DEL %s", "REFRESH1");
				r2 = redisCommand(k, "GET %s", "CRITICAL_LOAD");			//update critical load value
				cl = strtof(r2->str, NULL);
				cl = cl - p;								printf("New value of CRITICAL LOAD=%.2f\n", cl);
				r2 = redisCommand(k, "SET %s %.2f", "CRITICAL_LOAD", cl);
				// char transferTime[30] = "";
				// strcat(transferTime, "T_TM");
				// strcat(transferTime,splitMessage[1]);
				// reply = redisCommand(k, "GET %s", transferTime);
				// float tTm = strtof(reply->str, NULL);
				// r4 = redisCommand(k, "GET %s", "SOC");
				// float s = strtof(r4->str, NULL);
				// float p = strtof(splitMessage[2], NULL);
				// r5 = redisCommand(k, "GET %s", "CAPACITY");
				// float capacity = strtof(r5->str, NULL);
				// float reduced = p*tTm;
				// float sReduced = reduced/capacity*100;
				// s = s - (200*sReduced);
				// //s = 55;                                
				// FILE *fp;
				// fp = fopen("../log_files/soc.log", "w");
				// fprintf(fp, "%.2f\n", s); printf("SOC value increased to %.2f\n", s);
				// fclose(fp);
				// redisCommand(k, "SET %s %d", "SOC_A", 100);
				// redisCommand(k, "LPUSH %s %d", "SOC_IN", 1);
				// redisCommand(k, "SET %s %.2f", "SOC", 75.0);
				// redisCommand(k, "SET %s %.2f", "SOC_A", 75.0);			
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
				pub_sub("pub", selectedIpInPath, message, "0", "0");
			}
			else if(strcmp(r1->str, splitMessage[3]) == 0)										//SINK
			{	
				p = strtof(splitMessage[2], NULL);
				reply = redisCommand(k, "GET %s", splitMessage[j]);
				pA =  strtof(reply->str, NULL);
				pA = pA - p;																	//PORT POWER(+ if in and - if out)
				redisCommand(k, "SET %s %.2f", splitMessage[j], pA);printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
				redisCommand(k, "SET %s %s", "REFRESH", splitMessage[j]);
				redisCommand(k, "DEL %s", "REFRESH1");
				redisCommand(k, "DEL %s", "PEER_SELECTED");		
				// char transferTime[30] = "";
				// strcat(transferTime, "T_TM");
				// strcat(transferTime,splitMessage[1]);
				// reply = redisCommand(k, "GET %s", transferTime);
				// float tTm = strtof(reply->str, NULL);
				// r4 = redisCommand(k, "GET %s", "SOC");
				// float s = strtof(r4->str, NULL);
				// float p = strtof(splitMessage[2], NULL);
				// r5 = redisCommand(k, "GET %s", "CAPACITY");
				// float capacity = strtof(r5->str, NULL);
				// float added = p*tTm;
				// float sAdded = added/capacity*100;
				// s = s + (200*sAdded);
				// //s = 55;                                
				// FILE *fp;
				// fp = fopen("../log_files/soc.log", "w");
				// fprintf(fp, "%.2f\n", s); printf("SOC value increased to %.2f\n", s);
				// fclose(fp);
				// redisCommand(k, "LPUSH %s %d", "SOC_IN", 1);
				// redisCommand(k, "SET %s %d", "SOC_A", 100);				
			}
			else																				//INTERMEDIATE
			{			
				p = strtof(splitMessage[2], NULL);
				reply = redisCommand(k, "GET %s", splitMessage[j]);
				pA = strtof(reply->str, NULL);
				r2 = redisCommand(k, "GET %s", splitMessage[j-1]);
				pB = strtof(r2->str, NULL);
				pA = pA - p;																	//PORT POWER(+ if in and - if out)
				pB = pB + p;
				redisCommand(k, "SET %s %.2f", splitMessage[j], pA);printf("Power through port %s is %.2fW\n", splitMessage[j], pA);
				redisCommand(k, "SET %s %.2f", splitMessage[j-1], pB);printf("Power through port %s is %.2fW\n", splitMessage[j-1], pB);
				redisCommand(k, "SET %s %s", "REFRESH", splitMessage[j]);
				redisCommand(k, "SET %s %s", "REFRESH1", splitMessage[j-1]);
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
	}
}