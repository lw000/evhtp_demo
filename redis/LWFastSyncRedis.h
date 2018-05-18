/*
 * LWFastSyncRedis.h
 *
 *  Created on: Nov 3, 2017
 *      Author: root
 */

#ifndef LWFASTSYNCREDIS_H_
#define LWFASTSYNCREDIS_H_

#include <string>

struct redisContext;

class LWFastSyncRedis {
	redisContext *c;

public:
	LWFastSyncRedis();
	virtual ~LWFastSyncRedis();

public:
	int connect(const std::string& ip, int port =6379);
	int close();

public:
	void ping();

public:
	void setString(const std::string& key, const std::string& value);
	std::string getsetString(const std::string& key, const std::string& value);
	void setTimeoutString(const std::string& key, const std::string& value, int timeout);
	std::string getString(const std::string& key);

public:
	void setBinary(const std::string& key, const char* buff, int size);
	void getBinary(const std::string& key, char* buff, int size);

};

extern LWFastSyncRedis syncRedis;

#endif /* LWFASTSYNCREDIS_H_ */
