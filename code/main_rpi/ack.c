//gcc ack.c pub_sub.c -o ack -lhiredis -levent

#include "ack.h"


int main (int argc, char **argv)  
{
	redisReply *reply;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
			// handle error
	}
	char ackMessage[20];
	char splitMessage[24][16]; 
	char keyWord[5] = "";
	int i;
	while(1)
	{
		reply = redisCommand(k, "BLPOP %s %d", "ACK_BOARD", 0);
		redisCommand(k, "DECRBY %s %d", "NEW_ACK", 1);  
        strcpy(ackMessage, ""); 
        strcat(ackMessage, "ACK");
		char *r = reply->element[1]->str; 
        char *t = strtok(r, "\n");
        char *token = strtok(t, " ");
		i=0;        // max 24 fields each of max size 16
												   // loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
		strncpy(keyWord, splitMessage[0], 3);
		strcat(keyWord, "\0");
		strcat(ackMessage, splitMessage[0]);
		strcat(ackMessage, " ");
		strcat(ackMessage, splitMessage[1]);
		strcat(ackMessage, " ");
		reply = redisCommand(k, "GET %s", "PUB_SUB_IP");
		strcat(ackMessage, reply->str);
		if(strcmp(keyWord, "REQ")==0)
		{
			pub_sub("pub", splitMessage[2], ackMessage, "0", "0");
			printf("Published %s\n", ackMessage);
		}
		else
		{
			pub_sub("pub", splitMessage[3], ackMessage, "0", "0");
			printf("Published %s\n", ackMessage);
		}
	}
	return 0;
}
