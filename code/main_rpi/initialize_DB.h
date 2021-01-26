#ifndef INITIALIZE_DB_H_   /* Include guard */
#define INITIALIZE_DB_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "/home/pi/redis-stable/deps/hiredis/hiredis.h"
#include "peer_daemon.h"

char IP[16];
void socDaemon();
int initializeDb();

#endif // INITIALIZE_DB_H_