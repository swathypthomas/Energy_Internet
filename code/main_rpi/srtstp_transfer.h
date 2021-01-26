#ifndef SRTSTP_TRANSFER_H_   /* Include guard */
#define SRTSTP_TRANSFER_H_


#include <unistd.h>
#include <ctype.h>
#include "pub_sub.h"


void srtStpTransfer();
int intFrmStr(char *);
void publishSrtToSelected(char *, char *, char *, char *, char *, char *);
void publishStpToSelected(char *, char *, char *, char *, char *);


//shared by both srtstp_transfer.c and sense_disconnect.c
void publishStpToSelected(char *reqId, char *power, char *frmIp, char *toIp, char *ports)
{
	char message[500] = "STP";
	strcat(message, " ");
	strcat(message, reqId);
	strcat(message, " ");
	strcat(message, power);
	strcat(message, " ");
	strcat(message, frmIp);
	strcat(message, " ");
	strcat(message, toIp);
	strcat(message, " ");
	strcat(message, ports);
	pub_sub("pub", toIp, message, "0", "0");
}

#endif