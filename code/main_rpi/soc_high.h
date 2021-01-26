#ifndef SOC_HIGH_H_   /* Include guard */
#define SOC_HIGH_H_

#include <unistd.h>
#include "pub_sub.h"

void socHigh();
void publishAvlToPeers(char *, char *, char *, char *, char *, char *, char *);
void publishNavlToPeers(char *, char *, char *);
void publishAckReq(char *, char *, char *, char *);

#endif