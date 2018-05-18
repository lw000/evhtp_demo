/*
 * LWFastRedis.cpp
 *
 *  Created on: Nov 3, 2017
 *      Author: root
 */

#include "LWFastSyncRedis.h"
#include <stdio.h>
#include <string.h>

#include <hiredis/hiredis.h>

#define DEBUG_PRINT do{} while(0)

#if defined(DEBUG_PRINT)
#define DEBUG(...)\
        do {	\
            fprintf(stderr, "-----DEBUG-----\n");	\
            fprintf(stderr, "%s %s\n", __TIME__, __DATE__);	\
            fprintf(stderr, "%s:%d:%s():", __FILE__, __LINE__, __func__);	\
            fprintf(stderr, __VA_ARGS__);	\
        } while(0)
#endif


LWFastSyncRedis syncRedis;

LWFastSyncRedis::LWFastSyncRedis() :
		c(nullptr) {

}

LWFastSyncRedis::~LWFastSyncRedis() {

}

int LWFastSyncRedis::connect(const std::string& ip, int port) {
	struct timeval timeout = { 10, 500000 }; // 1.5 seconds
	c = redisConnectWithTimeout(ip.c_str(), port, timeout);
	if (c == nullptr || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		return -1;
	}
	return 0;
}

int LWFastSyncRedis::close() {
	/* Disconnects and frees the context */
	redisFree(c);
	return 0;
}

void LWFastSyncRedis::ping() {
	redisReply *reply = (redisReply *) redisCommand(c, "PING");
	printf("PING: %s\n", reply->str);
	freeReplyObject(reply);
}

void LWFastSyncRedis::setString(const std::string& key,
		const std::string& value) {
	redisReply *reply = (redisReply *) redisCommand(c, "SET %s %s", key.c_str(),
			value.c_str());

#ifdef DEBUG
	printf("SET: %s\n", reply->str);
#endif	//

	freeReplyObject(reply);
}

std::string LWFastSyncRedis::getsetString(const std::string& key,
		const std::string& value) {
	redisReply *reply = (redisReply *) redisCommand(c, "GETSET %s %s",
			key.c_str(), value.c_str());
	std::string r;
	if (reply->str != nullptr && reply->len > 0) {
		r.append(reply->str);
	}
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);
	return r;
}

void LWFastSyncRedis::setTimeoutString(const std::string& key,
		const std::string& value, int timeout) {
	timeout = (timeout <= 0) ? -1 : timeout;
	redisReply *reply = (redisReply *) redisCommand(c, "SETEX %s %d %s",
			key.c_str(), timeout, value.c_str());
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);
}

std::string LWFastSyncRedis::getString(const std::string& key) {
	redisReply *reply = (redisReply *) redisCommand(c, "GET %s", key.c_str());
	std::string r;
	if (reply->str != nullptr && reply->len > 0) {
		r.append(reply->str);
	}
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);
	return r;
}

void LWFastSyncRedis::setBinary(const std::string& key, const char* buff,
		int size) {
	const char *v[3];
	size_t vlen[3];
	v[0] = "SET";
	vlen[0] = strlen("SET");

	v[1] = key.c_str();
	vlen[1] = key.size();

	v[2] = (char*) buff;
	vlen[2] = size;
	redisReply *reply = (redisReply *) redisCommandArgv(c,
			sizeof(v) / sizeof(v[0]), v, vlen);
	printf("SET: %s\n", reply->str);
	freeReplyObject(reply);
}

void LWFastSyncRedis::getBinary(const std::string& key, char* buff, int size) {
	redisReply *reply = (redisReply *) redisCommand(c, "GET %s", key.c_str());

	memcpy(buff, reply->str, reply->len);

	freeReplyObject(reply);
}

