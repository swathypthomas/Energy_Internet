//gcc pub_sub.c -o pub_sub -lhiredis -levent
/*Example code for pub/sub
  run as publisher
  ./pub_sub pub <channelName> <message>       //here channel name is same as ip address

  run as subscriber
   ./pub_sub sub <channelName>*/

#include "pub_sub.h"
char count[14];
char board[14];

void onMessage(redisAsyncContext *c, void *reply, void *privdata) 
{
  redisReply *r = reply;
  if (reply == NULL) return;
  char msgbuf[500];
  int flag =0;
  if (r->type == REDIS_REPLY_ARRAY) 
  {
    for (int j = 0; j < r->elements; j++) 
    {
      if(j==0 && strcmp((r->element[j]->str), "message")==0)
      {
        flag = 1;
      }
      if(j==2 && flag==1)
      {
        char *message = r->element[j]->str;
        sprintf(msgbuf, "%s\n", message);
        redisContext *k = redisConnect("127.0.0.1", 6379); 
        if (k != NULL && k->err) 
        {
          printf("Error: %s\n", k->errstr);
        }
        redisReply *v;
        v = redisCommand(k,"INCRBY %s %d", count, 1);      //indicates that a new message is available
        v = redisCommand(k,"RPUSH %s %s", board, msgbuf);   
        char indexKey[5]="INII";
		char keyWord[5];
		char ackIdentifier[5];
	//char *keyWord = (char*) malloc(4);
        //char *indexKey = (char*) malloc(5);
        strncpy(indexKey, msgbuf, 4);
		strcat(indexKey, "\0"); 
        strncpy(keyWord, indexKey, 3);
		strcat(keyWord, "\0");
       // printf("\nkeyWord=%s.\n", keyWord);
        char selectedBoard[14] = "", selectedCount[14] = "";
        if(strcmp(keyWord, "REQ") == 0)
        {    //indicates that a new message for ACKNOWLEDGMENT is received
          redisCommand(k,"RPUSH %s %s", "ACK_BOARD", msgbuf); 
          redisCommand(k,"INCRBY %s %d", "NEW_ACK", 1); 
          if(strcmp(indexKey, "REQ0") == 0)
          {
          	printf("\nREQ DETECTED\n");
            strcpy(selectedBoard, "REQ_BOARD");
            strcpy(selectedCount, "NEW_REQ"); 
            redisCommand(k,"RPUSH %s %s", "REQ_BOARD_H", msgbuf);
          }
          else
          {
            strcpy(selectedBoard, "FWD_BOARD");
            strcpy(selectedCount, "NEW_FWD");     
            redisCommand(k,"RPUSH %s %s", "FWD_BOARD_H", msgbuf);           
          }
        }
        else if(strcmp(keyWord, "ACK") == 0)
        {
    			strncpy(ackIdentifier, msgbuf+3, 3);
    			strcat(ackIdentifier, "\0");
    			if(strcmp(ackIdentifier, "REQ")== 0)
    			{
    				printf("ACKREQ DETECTED\n");
    				strcpy(selectedBoard, "ACK_REQ_BOARD");
            strcpy(selectedCount, "NEW_ACKREQ"); 
            redisCommand(k,"RPUSH %s %s", "ACK_REQ_BOARD_H", msgbuf);
    			}
    			else if(strcmp(ackIdentifier, "SRT")==0)
    			{
    				printf("ACKSRT DETECTED\n");
    				strcpy(selectedBoard, "ACK_SRT_BOARD");
            strcpy(selectedCount, "NEW_ACKSRT"); 
            redisCommand(k,"RPUSH %s %s", "ACK_SRT_BOARD_H", msgbuf);
    			}
    			else if(strcmp(ackIdentifier, "STP")==0)
    			{
    				printf("ACKSTP DETECTED\n");
    				strcpy(selectedBoard, "ACK_STP_BOARD");
            strcpy(selectedCount, "NEW_ACKSTP"); 
            redisCommand(k,"RPUSH %s %s", "ACK_STP_BOARD_H", msgbuf);
    			}
        }
        else if(strcmp(keyWord, "AVL") == 0)
        {
          printf("\nAVL DETECTED\n");
          strcpy(selectedBoard, "AVL_BOARD");
          strcpy(selectedCount, "NEW_AVL");
          redisCommand(k,"RPUSH %s %s", "AVL_BOARD_H", msgbuf);
        }
        else if(strcmp(keyWord, "NAVL") == 0)
        {
          printf("\nNAVL DETECTED\n");
          strcpy(selectedBoard, "NAVL_BOARD");
          strcpy(selectedCount, "NEW_NAVL");
          redisCommand(k,"RPUSH %s %s", "AVL_BOARD_H", msgbuf);
        }
        else if(strcmp(keyWord, "SRT") == 0)
        {
          redisCommand(k,"RPUSH %s %s", "ACK_BOARD", msgbuf); 
          redisCommand(k,"INCRBY %s %d", "NEW_ACK", 1);      //indicates that a new message for ACKNOWLEDGMENT is received
          printf("\nSRT DETECTED\n");
          strcpy(selectedBoard, "SRT_BOARD");
          strcpy(selectedCount, "NEW_SRT");
          redisCommand(k,"RPUSH %s %s", "SRT_BOARD_H", msgbuf);
        }
        else if(strcmp(keyWord, "STP") == 0)
        {
          redisCommand(k,"RPUSH %s %s", "ACK_BOARD", msgbuf); 
          redisCommand(k,"INCRBY %s %d", "NEW_ACK", 1);       //indicates that a new message for ACKNOWLEDGMENT is received
          printf("\nSTP DETECTED\n");
          strcpy(selectedBoard, "STP_BOARD");
          strcpy(selectedCount, "NEW_STP");
          redisCommand(k,"RPUSH %s %s", "STP_BOARD_H", msgbuf);
        }
        if(strcmp(selectedBoard, "") != 0)
        {
          v = redisCommand(k,"RPUSH %s %s", selectedBoard, msgbuf);  
          v = redisCommand(k,"INCRBY %s %d", selectedCount, 1);      //indicates that a new message is available
        } 
        freeReplyObject(v);
        redisFree(k);
//	free(keyWord);
//	free(indexKey);
      }
    }
  }
}

int pub_sub (char *action, char *channel, char *message, char* cnt, char* brd) 
{
  strcpy(count, cnt);
  strcpy(board, brd);
  signal(SIGPIPE, SIG_IGN);
  //printf("READY TO PUBLISH 1\n");                              //trace
  struct event_base *base = event_base_new();
  if(strcmp(action, "sub") == 0)
  {
    redisAsyncContext *c = redisAsyncConnect(channel, 6379);
                              //trace
    if (c->err) 
    {
      printf("error: %s\n", c->errstr);
      return 1;
    }
    redisLibeventAttach(c, base);
    char s[26] = "SUBSCRIBE";
    strcat(s, " ");
    strcat(s, channel);      
    redisAsyncCommand(c, onMessage, NULL, s);
  }    
  else if(strcmp(action, "pub") == 0)
  {
   // printf("READY TO PUBLISH 2 CHANNEL:%s\n", channel);                              //trace
    redisContext *c1 = redisConnect(channel, 6379);
   // printf("READY TO PUBLISH 3\n");                              //trace
    if (c1->err) 
    {
      printf("error: %s\n", c1->errstr);
      return 1;
    }
    redisReply *reply;
    reply = redisCommand(c1,"PUBLISH %s %s",channel, message);
    printf("\npublished %s %s\n", channel, message);   //trace                
    char *t = strtok(message, "\n");
    strcat(t, " ");
    strcat(t, "SENT\n");
    redisCommand(c1, "LPUSH %s %s", "MESSAGE_BOARD", t);
    redisCommand(c1, "INCRBY %s %d", "NEW_MESSAGE", 1);
    freeReplyObject(reply);
    redisFree(c1);
  }
    else
      printf("Use pub or sub\n" );
    event_base_dispatch(base);
    return 0;
}
