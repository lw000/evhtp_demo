/*
 * redis_demo.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: lw
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis/hiredis.h>
#include "LWFastSyncRedis.h"

#if 0

int main(int argc, char **argv) {
	const char *hostname = (argc > 1) ? argv[1] : "192.168.204.128";
	int port = (argc > 2) ? atoi(argv[2]) : 6379;

	LWFastSyncRedis syncRedis;
	syncRedis.connect(hostname, port);

	/* PING server */
	syncRedis.ping();

	{
		std::string s1 = syncRedis.getsetString("name", "liwei");
		std::string s2 = syncRedis.getsetString("name", "heshanshan");
	}

	{
		typedef struct point {
			int a;
			double b;
			unsigned int c;
			char d;
		} point_t;

		for (int i = 10000; i < 20000; i++) {
			point_t vt;
			vt.a = i;
			vt.b = i;
			vt.c = i;
			vt.d = 'd';
			char buff[64];
			sprintf(buff, "binary:%d", i);
			syncRedis.setBinary(buff, (char*) &vt, sizeof(point_t));
			point_t vt1;
			syncRedis.getBinary(buff, (char*) &vt1, sizeof(point_t));
			printf("%d, %f, %u, %d \n", vt1.a, vt1.b, vt1.c, vt1.d);
		}
	}

	/* Set a key */
	{
		for (int i = 10000; i < 20000; i++) {
			char buff[64];
			sprintf(buff, "foo:%d", i);
			syncRedis.setString(buff, "hello world");
		}
	}

	/* Set a key */
	{
		for (int i = 10000; i < 20000; i++) {
			char buff[64];
			sprintf(buff, "session:%d", i);
			syncRedis.setTimeoutString(buff, "hello world", 600);
		}
	}
//
//	/* Set a key using binary safe API */
//	{
////		reply = (redisReply *) redisCommand(c, "SET %b %b", "bar", (size_t) 3,
////				"hello", (size_t) 5);
//
//		reply = (redisReply *) redisCommand(c, "SET %s %b", "bar", "hello",
//				(size_t) 5);
//
//		printf("SET (binary API): %s\n", reply->str);
//		freeReplyObject (reply);
//	}
//
//	{
//		/* Try a GET and two INCR */
//		reply = (redisReply *) redisCommand(c, "GET foo");
//		printf("GET foo: %s\n", reply->str);
//		freeReplyObject (reply);
//
//		reply = (redisReply *) redisCommand(c, "INCR counter");
//		printf("INCR counter: %lld\n", reply->integer);
//		freeReplyObject(reply);
//
//		/* again ... */
//		reply = (redisReply *) redisCommand(c, "INCR counter");
//		printf("INCR counter: %lld\n", reply->integer);
//		freeReplyObject(reply);
//	}
//
//	{
//		/* Create a list of numbers, from 0 to 9 */
//		reply = (redisReply *) redisCommand(c, "DEL mylist");
//		freeReplyObject (reply);
//
//		for (j = 0; j < 100; j++) {
//			char buf[64];
//			snprintf(buf, 64, "%d", j);
//
//			reply = (redisReply *) redisCommand(c, "LPUSH mylist element-%s",
//					buf);
//			freeReplyObject(reply);
//		}
//
//		/* Let's check what we have inside the list */
//		reply = (redisReply *) redisCommand(c, "LRANGE mylist 0 -1");
//		if (reply->type == REDIS_REPLY_ARRAY) {
//			for (j = 0; j < reply->elements; j++) {
//				printf("%u) %s\n", j, reply->element[j]->str);
//			}
//		}
//		freeReplyObject(reply);
//	}
//
//	/* Disconnects and frees the context */
//	redisFree (c);

	syncRedis.close();

	return 0;
}

#endif
