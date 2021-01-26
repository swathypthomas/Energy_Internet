//gcc srtstp_transfer.c pub_sub.c -o srtstp_transfer -lhiredis -levent
///Subscribes to the interface IP address and captures any message published to it.
///@note *Compiled with* gcc listen.c pub_sub.c -o /home/swathy/Documents/main_project/Energy_Internet/main/executables/listen -L/home/swathy/Downloads/softwares/redis-6.0.4/deps/hiredis/ -lhiredis -levent

#include "srtstp_transfer.h"


int main (int argc, char **argv)  
{
	srtStpTransfer();
	return 0;
}

//SRT MessageId Power frm_IP to_IP Port_list
void publishSrtToSelected(char *reqId, char *power, char *frmIp, char *toIp, char *ports, char *time)
{
	char message[500] = "SRT";
	strcat(message, " ");	
	strcat(message, reqId);
	strcat(message, " ");
	strcat(message, power);
	strcat(message, " ");
	strcat(message, frmIp);
	strcat(message, " ");
	strcat(message, toIp);
	strcat(message, " ");
	strcat(message, time);
	strcat(message, " ");
	strcat(message, ports);
	//strcat(message, "\n");
	//printf("X%sX\n", ports);
	pub_sub("pub", toIp, message, "0", "0");
}

//STP MessageId Power frm_IP to_IP Port_list
// void publishStpToSelected(char *reqId, char *power, char *frmIp, char *toIp, char *ports)
// {
// 	char message[500] = "STP ";
// 	strcat(message, " ");
// 	strcat(message, reqId);
// 	strcat(message, " ");
// 	strcat(message, power);
// 	strcat(message, " ");
// 	strcat(message, frmIp);
// 	strcat(message, " ");
// 	strcat(message, toIp);
// 	strcat(message, " ");
// 	strcat(message, ports);
// 	pub_sub("pub", toIp, message, "0", "0");
// }

void srtStpTransfer()
{
	redisReply *reply;	
	redisContext *k = redisConnect("127.0.0.1", 6379); 
	redisReply *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8;
	//SRT MessageId Power frm_IP to_IP Port_list
	//STP MessageId frm_IP to_IP Port_list
	while(1)
	{
		redisCommand(k, "BLPOP %s %d", "PEER_SELECTED", 0); 
		redisCommand(k, "LPUSH %s, %s", "PEER_SELECTED", "1");
		//printf("hii\n");
		s1 = redisCommand(k, "GET %s", "FROM_IP");
		s2 = redisCommand(k, "GET %s", "POWER");
		s3 = redisCommand(k, "GET %s", "PORT_PATH");
		s4 = redisCommand(k, "GET %s", "TRANSACTION_ID");
		s5 = redisCommand(k, "GET %s", "TIME");
		s6 = redisCommand(k, "SET %s %d", "TIMEOUT", 1);
		s8 = redisCommand(k, "GET %s", "PUB_SUB_IP");//printf("hii%s\n", s5->str);
		int t1 = intFrmStr(s5->str);
		printf("START transfer for %d seconds\n", t1);
		redisContext *c = redisConnect(s1->str, 6379); 
		publishSrtToSelected(s4->str, s2->str, s8->str, s1->str, s3->str, s5->str);
		//WAIT for power supply. Once detected continue. else wait for 2 seconds and again sent SRT.
		//If no reply after 5 tries, go back to MonitorSOC()
		//sleep(t1);     //actually this should be the timeout 
		float t = t1/3;
		int i;
		int noReply = 0;
		float totalTimeTaken = 0;
		float s, p, capacity, added, sAdded;
		reply = redisCommand(k, "GET %s", "CAPACITY");
		capacity = strtof(reply->str, NULL);                                         
		FILE *fp, *fpN;
		fp = fopen("../log_files/soc.log", "w");
		reply = redisCommand(k, "GET %s", "SOC");
		int flg = 0;
		s = strtof(reply->str, NULL);
		p = strtof(s2->str, NULL);
		for(i=1; i<=3; i++)
		{
			usleep(t*1000000);
			c = redisConnect(s1->str, 6379);
			if (c != NULL && c->err) 
			{
				printf("Connection lost with %s: %s\n", s1->str, c->errstr);
				// handle error
				noReply = 1;
				break;
			} 		
			added = p*t;
			sAdded = added/capacity*100;
			s = s + (100*sAdded);
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
			totalTimeTaken += t;			
		}
		redisCommand(k, "SET %s %d", "SOC_A", 100);	
		redisCommand(k, "SET %s %s", "DISCARD_SOC_UPDATE", "0");
		redisCommand(k, "LPUSH %s %d", "SOC_IN", 1);
		printf("SOC value increased to %.2f. Power transferred for %.2fseconds\n", s, totalTimeTaken);
		char transferTime[20] = "";
		strcat(transferTime, "T_TM");
		strcat(transferTime,s4->str);
		redisCommand(k, "SET %s %.2f", transferTime, totalTimeTaken);
		if(noReply == 0)
		{
			publishStpToSelected(s4->str, s2->str, s8->str, s1->str, s3->str);
		}		//Here assuming that SOC increased to 60%
		else    //asking self to stop since not expecting the other side to communicate anymore
		{
			publishStpToSelected(s4->str, s2->str, s1->str, s8->str, s3->str);			
			// char stpReceived[13] = "";
			// strcat(stpReceived, "STP");
			// strcat(stpReceived, s4->str);
			// reply =redisCommand(k, "GET %s", stpReceived);
			// if(!(reply->str))
			// {
			// 	redisCommand(k, "SET %s %d", stpReceived, 1);		printf("\nPower transferred for %f seconds\n", t*i);
			// 	s5 = redisCommand(k, "GET %s", "SOC");
			// 	float s = strtof(s5->str, NULL);
			// 	float p = strtof(s2->str, NULL);
			// 	s1 = redisCommand(k, "GET %s", "CAPACITY");
			// 	float capacity = strtof(s1->str, NULL);
			// 	float added = p*t*i;
			// 	float sAdded = added/capacity*100;
			// 	s = s + (200*sAdded);
			// 	//s = 55;                                
			// 	FILE *fp;
			// 	fp = fopen("../log_files/soc.log", "w");
			// 	fprintf(fp, "%.2f\n", s);
			// 	fclose(fp);
			// 	redisCommand(c, "SET %s %d", "SOC_A", s);
			// 	redisCommand(c, "LPUSH %s %d", "SOC_IN", 1);

			// }
			// else
			// {
			// 	redisCommand(k, "INCRBY %s %d", stpReceived, 1);
			// }			
			// redisCommand(k, "EXPIRE %s %d", stpReceived, 30);
		}			
	}
	freeReplyObject(reply);	
	redisFree(k);
	freeReplyObject(s1);	
	freeReplyObject(s2);
	freeReplyObject(s3);	
	freeReplyObject(s4);
	freeReplyObject(s5);	
	freeReplyObject(s6);
	freeReplyObject(s7);	
	freeReplyObject(s8);
}


int intFrmStr(char *p)
{
	int val;
	while (*p) 
	{ // While there are more characters to process...
	    if ( isdigit(*p) || ( (*p=='-'||*p=='+') && isdigit(*(p+1)) )) 
	    {
	        // Found a number
	        val = strtol(p, &p, 10); // Read number
	    } else 
	    {
	        // Otherwise, move on to the next character.
	        p++;
	    }
	}
	return val;
}