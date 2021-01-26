//gcc peer_daemon.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/peer_daemon -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis

#include "peer_daemon.h"

int refreshPeerIpPorts()
//int main()
{
	int pid1, ret1, status;
    char port[2];
    char ip[16];
    redisReply *reply;    
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c != NULL && c->err) 
    {
       printf("Error: %s %s\n", c->errstr, "127.0.0.1");
    } 
    reply = redisCommand(c,"GET %s", "PUB_SUB_IP");
    char myIp[17];
    strcpy(myIp, reply->str);
    redisContext *k = redisConnect(myIp, 6379); 
    if (k != NULL && k->err) 
    {
       printf("Error: %s %s\n", k->errstr, myIp);
    }
    char *myArgs[] = {NULL};
    char *myEnv[] = {NULL};
    pid1 = fork();
    char cmd[100];
    if(pid1 == 0)
    {
    	reply = redisCommand(c, "GET %s", "SWITCH_IP");
    	snprintf(cmd, sizeof(cmd), "./switch_ARP %s;exit", reply->str); 
    	//printf("%s\n", cmd);
    	system(cmd);    	
    	reply = redisCommand(c, "GET %s", "INTERFACE");
    	snprintf(cmd, sizeof(cmd), "arp-scan --interface=%s --localnet > mac_ip;exit", reply->str);
    	//printf("%s\n", cmd);
    	system(cmd);
    	system(cmd);
    }
    ret1 = waitpid (pid1, &status, 0);
    if((ret1 == pid1))
    {
    	reply = redisCommand(c, "HKEYS %s", "PEER_SWITCH_IP");
		if (reply->type == REDIS_REPLY_ARRAY) 
		{
			remove("ip_port");
			for (int i = 0; i < reply->elements; i++) 
			{
				char *peerSwitchIp = reply->element[i]->str;
    			snprintf(cmd, sizeof(cmd), "./ip_port.pl %s;exit", peerSwitchIp);
    			//printf("%s\n", cmd);
    			system(cmd);
    		}
    	}
    }
    FILE* fp = fopen("peer_ip_port","r");
    FILE* fs = fopen("local_ip_port","r");
    // if( fp == NULL)
    // {
    //     printf("No peer router\n");
    // }    
    // if( fs == NULL)
    // {
    //     printf("No LOAD/SOURCE\n");
    // }
    char ipPortMap[19];
    reply = redisCommand(c, "DEL %s", "PEER:IP:PORT");
    reply = redisCommand(c, "DEL %s", "LOAD:SOURCE:LIST");
    while(fscanf(fp,"%s %s\n", ip, port) > 0)
    {   
        reply = redisCommand(c,"HSET %s %s %s","PEER:IP:PORT", ip, port);
        redisContext *p = redisConnect(ip, 6379);
        redisCommand(p, "SADD %s %s","PEER", myIp);
        redisFree(p);
    }
    while(fscanf(fs,"%s %s\n", ip, port) > 0)
    {       
        reply = redisCommand(c,"HSET %s %s %s","LOAD:SOURCE:LIST", ip, port);        
    }
    redisCommand(c,"DEL %s", "CONNECTION_UPDATE");
    redisCommand(c, "LPUSH %s %s", "CONNECTION_UPDATE", "1");
    fclose(fs);
    fclose(fp);
    remove("peer_ip_port");
    remove("local_ip_port");
    return(0);
}