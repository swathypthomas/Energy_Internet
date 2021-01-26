//gcc display_board.c -o display_board -lhiredis

#include "display_board.h"

int main (int argc, char **argv)  
{
	redisContext *k = redisConnect("127.0.0.1", 6379); 
    if (k->err) 
    {
      printf("error: %s\n", k->errstr);
      return 1;
    }
    redisReply *reply, *r1, *r2;
	char displayMessage[1650] = "";
	char splitMessage[24][16];                   
	int i, f;
	char keyWord[5] = "";
	char reqKeyWord[6] = "";
	while(1)
	{		
		memset(splitMessage, 0, sizeof splitMessage);     //zeroing out the 2d array
		if(strlen(displayMessage) > 1500)
		{
			strcpy(displayMessage, "");
		}
		reply = redisCommand(k,"BLPOP %s %d","MESSAGE_BOARD", 0);
		redisCommand(k, "DECRBY %s %d", "NEW_MESSAGE", 1);
		char *t = strtok(reply->element[1]->str, "\n");             
		char *token = strtok(t, " ");
		i=0;
			// loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
		f = 0;
		if(strcmp(splitMessage[i-1], "SENT") == 0)
		{
			f = 1;
		}
		strcpy(keyWord, "");
		strcpy(reqKeyWord, "");
		if(strlen(splitMessage[0])>=4)
		{
			strncpy(reqKeyWord, splitMessage[0], 4);
			strcat(reqKeyWord, "\0");
		}
		strncpy(keyWord, splitMessage[0], 3);
		strcat(keyWord, "\0");
		if(strlen(splitMessage[0]) < 5)
		{
			if(strcmp(keyWord, "REQ") == 0)
			{
				redisContext *c = redisConnect(splitMessage[2], 6379);
				reply = redisCommand(c, "GET %s", "ID");
				strcat(displayMessage, "Requesting ");
				strcat(displayMessage, splitMessage[3]);
				strcat(displayMessage, "Watt power for duration of ");
				strcat(displayMessage, splitMessage[4]);
				strcat(displayMessage, "Seconds from ");
				strcat(displayMessage, reply->str);
				strcat(displayMessage, ". ");
				redisFree(c);
				if((f == 0) && (strcmp(reqKeyWord, "REQ0") != 0))
				{
					strcat(displayMessage, "The message is geting forwarded to peer routers.");
				}
				strcat(displayMessage, "The message ID is ");
				strcat(displayMessage, splitMessage[1]);
				strcat(displayMessage, ".");
			}
			else if(strcmp(keyWord, "AVL") == 0)
			{
				redisContext *c = redisConnect(splitMessage[5], 6379);
				reply = redisCommand(c, "GET %s", "ID");
				strcat(displayMessage, "Ready to supply ");
				strcat(displayMessage, splitMessage[4]);
				strcat(displayMessage, "Watt power for duration of ");
				strcat(displayMessage, splitMessage[3]);
				strcat(displayMessage, "Seconds from ");
				strcat(displayMessage, reply->str);
				strcat(displayMessage, ". ");
				strcat(displayMessage, "Rupees ");
				strcat(displayMessage, splitMessage[2]);
				strcat(displayMessage, "-/ per Watt power quoted. ");
				strcat(displayMessage, "The message ID is ");
				strcat(displayMessage, splitMessage[1]);
				strcat(displayMessage, ".");
				redisFree(c);
			}
			else if(strcmp(keyWord, "NAV"))
			{
				redisContext *c = redisConnect(splitMessage[2], 6379);
				reply = redisCommand(c, "GET %s", "ID"); 	
				strcat(displayMessage, "Power not available for transfer from ");
				strcat(displayMessage, reply->str);
				strcat(displayMessage, ". ");
				strcat(displayMessage, "The message ID is ");
				strcat(displayMessage, splitMessage[1]);
				strcat(displayMessage, ".");
				redisFree(c);
			}
			else if(strcmp(keyWord, "SRT"))
			{  
				redisContext *c1 = redisConnect(splitMessage[3], 6379);
				r1 = redisCommand(c1, "GET %s", "ID");  	
				redisContext *c2 = redisConnect(splitMessage[4], 6379);
				r2 = redisCommand(c2, "GET %s", "ID"); 
				strcat(displayMessage, "Start ");
				strcat(displayMessage, splitMessage[2]);
				strcat(displayMessage, "Watt transfer from ");
				strcat(displayMessage, r1->str);
				strcat(displayMessage, " to ");
				strcat(displayMessage, r2->str);
				strcat(displayMessage, ". ");
				strcat(displayMessage, "The message ID is ");
				strcat(displayMessage, splitMessage[1]);
				strcat(displayMessage, ".");
				redisFree(c1);
				redisFree(c2);
			}
			else if(strcmp(keyWord, "STP"))
			{  
				redisContext *c1 = redisConnect(splitMessage[3], 6379);
				r1 = redisCommand(c1, "GET %s", "ID");  	
				redisContext *c2 = redisConnect(splitMessage[4], 6379);
				r2 = redisCommand(c2, "GET %s", "ID"); 
				strcat(displayMessage, "Stop ");
				strcat(displayMessage, splitMessage[2]);
				strcat(displayMessage, "Watt transfer from ");
				strcat(displayMessage, r1->str);
				strcat(displayMessage, " to ");
				strcat(displayMessage, r2->str);
				strcat(displayMessage, ". ");
				strcat(displayMessage, "The message ID is ");
				strcat(displayMessage, splitMessage[1]);
				strcat(displayMessage, ".");
				redisFree(c1);
				redisFree(c2);
			}
		}		
		else if(strlen(splitMessage[0]) > 5)
		{
			// char ackKeyWord[8];
			// strncpy(ackKeyWord, splitMessage[0], 6);
			// strcat(ackKeyWord, "\0");
			// if(strcmp(ackKeyWord, "ACKREQ"))
			// {
			// 	redisContext *c = redisConnect(splitMessage[2], 6379);
			// 	reply = redisCommand(c, "GET %s", "ID");    		
			// 	strcat(displayMessage, "Acknowledgement from ");
			// 	strcat(displayMessage, reply->str);
			// 	strcat(displayMessage, " for request message with Message ID ");
			// 	strcat(displayMessage, splitMessage[1]);
			// 	strcat(displayMessage, ".");
			// 	redisFree(c);
			// }
			// else if(strcmp(ackKeyWord, "ACKSRT"))
			// {
			// 	redisContext *c = redisConnect(splitMessage[2], 6379);
			// 	reply = redisCommand(c, "GET %s", "ID");    		
			// 	strcat(displayMessage, "Acknowledgement from ");
			// 	strcat(displayMessage, reply->str);
			// 	strcat(displayMessage, " for start power transfer message with Message ID ");
			// 	strcat(displayMessage, splitMessage[1]);
			// 	strcat(displayMessage, ".");
			// 	redisFree(c);
			// }
			// else if(strcmp(ackKeyWord, "ACKSTP"))
			// {
			// 	redisContext *c = redisConnect(splitMessage[2], 6379);
			// 	reply = redisCommand(c, "GET %s", "ID");    		
			// 	strcat(displayMessage, "Acknowledgement from ");
			// 	strcat(displayMessage, reply->str);
			// 	strcat(displayMessage, " for stop power transfer message with Message ID ");
			// 	strcat(displayMessage, splitMessage[1]);
			// 	strcat(displayMessage, ".");
			// 	redisFree(c);
			// }    	
		}
		strcat(displayMessage, "       "); //printf("displaymessge=%s\n", displayMessage);
		if(f==1)
		{
			redisCommand(k, "DEL %s", "OUT_BOARD");
			redisCommand(k, "LPUSH %s %s", "OUT_BOARD", displayMessage);
		}
		else
		{
			redisCommand(k, "DEL %s", "IN_BOARD");
			redisCommand(k, "LPUSH %s %s", "IN_BOARD", displayMessage);
		}
	}    
    redisFree(k);
	return 0;
}