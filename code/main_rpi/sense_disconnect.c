//At source ER, if during the committed duration of power transfer, the sink ER gets not reachable,
//the power transfer is stopped at self, taking into account only the transfer time(T_TM) till that instant.

#include "sense_disconnect.h"


int main (int argc, char **argv)  
{
    redisReply *reply;	
	redisContext *k = redisConnect("127.0.0.1", 6379);
    while(1)
    {
        reply = redisCommand(k, "BLPOP %s %d", "TRACK_POWER_OUT", 0);
    	char splitMessage[24][16];
        int i;
        char *srtCommand = reply->element[1]->str;
		char *t = strtok(srtCommand, "\n");
		char *token = strtok(t, " ");						   // loop through the string to extract all other tokens
		while( token != NULL ) 
		{
			sprintf(splitMessage[i], "%s", token ); //printing each token
			token = strtok(NULL, " ");
			i++;
		}
        float tD = strtof(splitMessage[5], NULL);
        redisContext *c = redisConnect(splitMessage[3], 6379);
        int slot;
        int noReply = 0;
        float tDF = tD/3;
		float tT = 0;
		float s, p, capacity, reduced, sReduced;
		reply = redisCommand(k, "GET %s", "CAPACITY");
		capacity = strtof(reply->str, NULL);                               
		FILE *fp, *fpN;
		reply = redisCommand(k, "GET %s", "SOC");
		p = strtof(splitMessage[2], NULL);
		s = strtof(reply->str, NULL);
		int flg = 0;
        for(slot=1; slot<=3; slot++)
        {
            usleep(tDF*1000000);
			c = redisConnect(splitMessage[3], 6379);
			if (c != NULL && c->err) 
			{
				printf("Connection lost with %s: %s\n", splitMessage[3], c->errstr);
				// handle error
				noReply = 1;
				break;
			} 
			reduced = p*tDF;
			sReduced = reduced/capacity*100;
			s = s - (100*sReduced);
				//s = 55; 
			if(flg==0)
			{
				fp = fopen("../log_files/soc.log", "w");
				fprintf(fp, "%.2f\n", s);
				fclose(fp);
				flg = 1; 			
			}
			else if(flg==1)
			{
				fpN = fopen("../log_files/soc.log", "w");
				fprintf(fpN, "%.2f\n", s);
				fclose(fpN);
				flg = 0; 
			}			
			redisCommand(k, "SET %s %s", "DISCARD_SOC_UPDATE", "1");
			redisCommand(k, "LPUSH %s %d", "SOC_IN", 1);
            tT += tDF;
        }
		redisCommand(k, "SET %s %d", "SOC_A", 100);	
		redisCommand(k, "SET %s %s", "DISCARD_SOC_UPDATE", "0");
		redisCommand(k, "LPUSH %s %d", "SOC_IN", 1);
		printf("SOC value increased to %.2f. Power transferred for %.2fseconds\n", s, tT);
		char transferTime[20] = "";
		strcat(transferTime, "T_TM");
		strcat(transferTime,splitMessage[1]);
		redisCommand(k, "SET %s %.2f", transferTime, tT);
        if(noReply == 1)
        {
            publishStpToSelected(splitMessage[1], splitMessage[2], splitMessage[3], splitMessage[4], splitMessage[5]);           
        }
    }
	freeReplyObject(reply);	
	redisFree(k);
	return 0;
}		
        
        
