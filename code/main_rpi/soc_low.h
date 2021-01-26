#ifndef SOC_LOW_H_   /* Include guard */
#define SOC_LOW_H_

#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "pub_sub.h"

void socLow();
int rnGn();
void publishReqToPeers(int , char *, char *, char *, char *);
void retransmitReq(int , char *, char *, char *);
int intFrmStr(char *);

#endif