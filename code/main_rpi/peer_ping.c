//gcc peer_daemon.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/peer_daemon -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis

#include "peer_ping.h"
#include "peer_daemon.h"

int main (int argc, char **argv)  
{
	peerPing();
	return 0;
}

int peerPing()
//int main()
{
    redisContext *k = redisConnect("127.0.0.1", 6379);
    redisReply *reply, *r;
    reply = redisCommand(k, "GET %s", "PUB_SUB_IP");
    redisContext *c = redisConnect(reply->str, 6379);
    redisContext *cC;
    char ip[17]; 
    int n, i;
    while(1)
    {
        reply = redisCommand(k, "HKEYS %s", "PEER:IP:PORT"); 
        r = redisCommand(k, "HLEN %s", "PEER:IP:PORT");
        n = r->integer;
        for(i=0; i<n; i++)
        {
            strcpy(ip, reply->element[i]->str); //printf("peer IPs :-%s\n", ip);
	        cC = redisConnect(ip, 6379);//cC represents connection_checking
            if (cC != NULL && cC->err) 
			{
				redisCommand(k, "HDEL %s %s", "PEER:IP:PORT", ip);
                redisCommand(k,"DEL %s", "CONNECTION_UPDATE");
                redisCommand(k, "LPUSH %s %s", "CONNECTION_UPDATE", "1");
				redisCommand(c, "SREM %s %s", "PEER", ip);
			} 
        }
        r = redisCommand(c, "SCARD %s", "PEER"); 
        redisCommand(c, "DEL %s", "PEER");
        n = r->integer; //printf("new peers %d\n", n);
        if(n > 0)
        {
            refreshPeerIpPorts();
            redisCommand(k,"DEL %s", "CONNECTION_UPDATE");
            redisCommand(k, "LPUSH %s %s", "CONNECTION_UPDATE", "1");
        }
        sleep(4);
    }
    return(0);
}