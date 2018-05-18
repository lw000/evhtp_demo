//============================================================================
// Name        : redis_demo.cpp
// Author      : lw
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#if 0

#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

#include <event2/thread.h>

typedef struct thread_shared_redis {
	redisAsyncContext *c;
	int connected;
} thread_shared_redis_t;

static thread_shared_redis_t shared_redis = { NULL, 0 };
std::mutex __g_mutex;
std::condition_variable __g_cv; // 全局条件变量.

static void setCallback(redisAsyncContext *c, void *r, void *privdata) {
	redisReply *reply = (redisReply *) r;

	if (reply == nullptr)
		return;

	if (!(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0)) {

		printf("failed to execute command[%s]\n", (char*) privdata);
		freeReplyObject(reply);
		redisAsyncFree(c);
		return;
	}

	printf("command [%s] >>>>>>>> reply: %s\n", (char*) privdata, reply->str);
}

static void getCallback(redisAsyncContext *c, void *r, void *privdata) {
	redisReply *reply = (redisReply *) r;
	if (reply == nullptr)
		return;

	if (reply->type == REDIS_REPLY_STRING) {
		printf("command [%s] >>>>>>>> str[%d]: %s\n", (char*) privdata,
				reply->len, reply->str);
		/* Disconnect after receiving the reply to GET */
		//	redisAsyncDisconnect(c);
	}
}

static void connectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("error: %s\n", c->errstr);
		return;
	}

	shared_redis.c = const_cast<redisAsyncContext*>(c);
	shared_redis.connected = 1;

	__g_cv.notify_all();

	printf("connected...\n");
}

static void disconnectCallback(const redisAsyncContext *c, int status) {
	if (status != REDIS_OK) {
		printf("Error: %s\n", c->errstr);
		return;
	}

	printf("disconnected...\n");
}

void __run_redis_server(void* self) {

	thread_shared_redis_t * s = (thread_shared_redis_t*) self;

	{
		std::unique_lock<std::mutex> l(__g_mutex);
		if (s->connected != 1) {
			__g_cv.wait(l);
		}
	}

	while (1) {
//		{
//			struct VT {
//				int a;
//				double b;
//				unsigned int c;
//				char d;
//			};
//
//			VT vt;
//			vt.a = 100;
//			vt.b = 100.121;
//			vt.c = 100;
//			vt.d = 'd';
//
//			const char *v[3];
//			size_t vlen[3];
//			v[0] = "SET";
//			vlen[0] = strlen("SET");
//
//			v[1] = "C";
//			vlen[1] = strlen("C");
//
//			v[2] = (char*) &vt;
//			vlen[2] = sizeof(vt);
//
//			int c0 = redisAsyncCommandArgv(s->c, setCallback, (void*) "set",  sizeof(v) / sizeof(v[0]), v, vlen);
//			int c1 = redisAsyncCommandArgv(s->c, getCallback, (void*) "get", "GET C", strlen("GET C"));
//		}

		{
			std::string k = "name";
			std::string v = "liwei";
			for (int i = 10000; i < 20000; i++) {
				int c0 = redisAsyncCommand(s->c, setCallback, (void*) "set",
						"SET %s:%d %s", k.c_str(), i, v.c_str());
				printf("%d", c0);
				int c1 = redisAsyncCommand(s->c, getCallback, (void*) "get",
						(char*) "GET %s:%d", k.c_str(), i);
				printf("%d", c1);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);

#if defined(__WIN32) || defined(WIN32)
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif

	{
		std::thread t(__run_redis_server, &shared_redis);
		t.detach();
	}

	struct event_base *base = event_base_new();
	redisAsyncContext *c = redisAsyncConnect("192.168.204.128", 6379);
	if (c->err) {
		/* Let *c leak for now... */
		printf("error: %s\n", c->errstr);
		return 1;
	}

	int r = redisLibeventAttach(c, base);
	redisAsyncSetConnectCallback(c, connectCallback);
	redisAsyncSetDisconnectCallback(c, disconnectCallback);
//	r = event_base_dispatch(base);
	r = event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);

	printf("r = %d\n", r);

	return 0;
}

#endif
