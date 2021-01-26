#ifndef PUB_SUB_H_   /* Include guard */
#define PUB_SUB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "/home/pi/redis-stable/deps/hiredis/hiredis.h"
#include "/home/pi/redis-stable/deps/hiredis/async.h"
#include "/home/pi/redis-stable/deps/hiredis/adapters/libevent.h"

void onMessage(redisAsyncContext *, void *, void *);
int pub_sub (char *, char *, char *, char *, char *);


#endif