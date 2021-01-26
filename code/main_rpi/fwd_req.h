#ifndef FWD_REQ_H_   /* Include guard */
#define FWD_REQ_H_


#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "pub_sub.h"

void fwdReq();
bool wildCardCmp(char *, char *);
int intFrmStr(char *);
void forwardReqToPeers(char *, char *, char *, char *, char *, char *, char *);


#endif