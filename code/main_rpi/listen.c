///Subscribes to the interface IP address and captures any message published to it.
///@note *Compiled with* gcc listen.c pub_sub.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/listen -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis -levent

#include "listen.h"

int main (int argc, char **argv)  
{
	listenContinuously();
	return 0;
}

///Continuously subscribed to interface IP address.
///
///It resets the count of message to 0 initially and then calls pub_sub() with subsciption as paratmeter.
///@see pub_sub()
void listenContinuously()  
{
	redisReply *reply;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	if (k != NULL && k->err) 
	{
		printf("Error: %s\n", k->errstr);
		// handle error
	}
	reply = redisCommand(k, "GET %s", "PUB_SUB_IP");
	char *listenIp = (reply->str);
	
	char *cnt = "NEW_MESSAGE";
	char *brd = "MESSAGE_BOARD";
	reply = redisCommand(k, "SET %s %d", cnt, 0);
	reply = redisCommand(k, "SET %s %d", "NEW_REQ", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_FWD", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_AVL", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_SRT", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_STP", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_ACK", 0);	
	reply = redisCommand(k, "SET %s %d", "NEW_ACKREQ", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_ACKSRT", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_ACKSTP", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_OUT", 0);
	reply = redisCommand(k, "SET %s %d", "NEW_IN", 0);
	reply = redisCommand(k, "DEL %s", brd);

	pub_sub ("sub", listenIp, NULL, cnt, brd);  //replace the ip address by listenIp here
}