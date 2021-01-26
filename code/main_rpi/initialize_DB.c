/*
 * C Program to Get IP Address and insert it as SELF_IP to redis-server
 * Also set name of this router. example "ER1"
 * gcc initialize_DB.c peer_daemon.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/peer_daemon -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis
 */

#include "initialize_DB.h"

int main (int argc, char **argv)  
{
	initializeDb();
		return 0;
}


void socDaemon()
{
	redisContext *c = redisConnect("127.0.0.1", 6379); 
	if (c != NULL && c->err) 
	{
		printf("Error: %s\n", c->errstr);
				// handle error
	}
	redisReply *reply, *reply1, *r;
	FILE *fp= NULL;
	float soc;
	while(1)
	{
		reply = redisCommand(c, "BLPOP %s %d", "SOC_IN", 0);
		r = redisCommand(c, "GET %s", "DISCARD_SOC_UPDATE");
		fp = fopen ("../log_files/soc.log", "r");	
		fscanf(fp, "%f", &soc);
		//printf("%.2f\n", soc);
		fclose(fp);
		redisCommand(c, "DEL %s %s", "LOWER_THRESHOLD", "UPPER_THRESHOLD");
		if((soc < 50) && (strcmp(r->str, "0") == 0))
		{
			redisCommand(c, "LPUSH %s %d", "LOWER_THRESHOLD", 1);
		}
		else if((soc > 70) && (strcmp(r->str, "0") == 0))
		{
			reply1 = redisCommand(c, "GET %s", "SOC_A");
			if(!(reply1->str));
			else
			{
				float soc_a = strtof(reply1->str, NULL);
				if(soc_a < soc)
				{
					soc = soc_a;
				}
			}
			if(soc > 70)
			{
				redisCommand(c, "LPUSH %s %d", "UPPER_THRESHOLD", 1);
			}
		}
		reply = redisCommand(c, "SET %s %.2f", "SOC", soc);
		float tarrif = 50 + 20*(1-soc/100);                       //cost function. x+y(1-soc). Take x=50 and y=20 to get cost variation of 50 to 70 when soc changes from 100% to 0%.
		reply = redisCommand(c, "SET %s %.2f", "TARRIF", tarrif);
		//break;            //remove this when SOC value is dynamic
		//sleep(1);
	}
	//printf("%f\n", soc);
	freeReplyObject(reply);
	redisFree(c);
}


int initializeDb()
{
    int n;
	struct ifreq ifr;	
    char array[] = "eth0"; //change the interface name according to the system
    n = socket(AF_INET, SOCK_DGRAM, 0);
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IF_NAMESIZE - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
    strcpy(IP, (char *)(inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr)) );
	redisContext *k = redisConnect("127.0.0.1", 6379); 
    if (k != NULL && k->err) 
    {
		printf("Error: %s\n", k->errstr);
	}
	redisReply *reply;
	reply = redisCommand(k,"FLUSHALL");
	reply = redisCommand(k,"SET %s %s","ID", "ER2");
	float capacity = 84.0*3600;   //watt seconds
	reply = redisCommand(k, "SET %s %.2f", "CAPACITY", capacity);
	float criticalLoad = 12.0*3600; //0.5A at 12V gives 6W. To support two such loads, 12W.
	reply = redisCommand(k, "SET %s %.2f", "CRITICAL_LOAD", criticalLoad/3600);
	float p = criticalLoad + (0.1*capacity);
	float wattSecond = p/3600;					//when soc falls to 50%, it requests for power to meet its critical load, as well as to charge till 60%
	reply = redisCommand(k, "SET %s %.2f", "POWER_REQUEST", wattSecond); // = 20.4, corresponds to 1.7A
	reply = redisCommand(k, "SET %s %d", "TIME_REQUEST", 10);
	reply = redisCommand(k, "SET %s %d", "NEW_MESSAGE", 0);
	reply = redisCommand(k, "LPUSH %s %s", "MESSAGE_BOARD", "HELLO");
	reply = redisCommand(k, "SET %s %s", "PUB_SUB_IP", IP); //now subscribe to this IP
	redisContext *c = redisConnect(IP, 6379);
	redisCommand(c,"SET %s %s","ID", "ER2");
	redisFree(c);
	reply = redisCommand(k, "SET %s %d", "REFRESH", 0);  //value updated with each change in power through a port
	reply = redisCommand(k, "SET %s %s", "SWITCH_IP", "192.168.0.60");
	reply = redisCommand(k, "SET %s %s", "INTERFACE", array);
	reply = redisCommand(k, "SET %s %.2f", "SOC_A", 100.0);
	reply = redisCommand(k, "SET %s %.2f", "1", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "2", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "3", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "4", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "5", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "6", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "7", 0.0);
	reply = redisCommand(k, "SET %s %.2f", "8", 0.0);
	reply = redisCommand(k, "SET %s %d", "TIMESLOT", 20);
	reply = redisCommand(k, "HSET %s %s %s", "PEER_SWITCH_IP", "192.168.0.40", "0"); //add more lines of HSET to add more peer switches
	redisCommand(k, "SET %s %s", "DISCARD_SOC_UPDATE", "0");
	redisCommand(k,"LPUSH %s %.2f", "SOC_IN", 0.0);
	//reply = redisCommand(k, "SET %s %s", "PEER_SELECTED", "0");
	refreshPeerIpPorts();
	freeReplyObject(reply);
	redisFree(k);
	socDaemon();
    return 0;
}