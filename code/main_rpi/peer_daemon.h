#ifndef PEER_DAEMON_H_   /* Include guard */
#define PEER_DAEMON_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "/home/pi/redis-stable/deps/hiredis/hiredis.h"

int refreshPeerIpPorts();
#endif