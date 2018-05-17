/*
 * bunissfunc.cpp
 *
 *  Created on: May 16, 2018
 *      Author: root
 */

#include "bunissfunc.h"

#include "ConverCode.h"
#include "data.h"

#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>

#include <log4z/log4z.h>
using namespace zsummer::log4z;

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filestream.h"


#include "LWFastSyncRedis.h"

long long guid = 10000;
std::mutex user_table_lock;
std::mutex user_lock;

static int print_kvs(evhtp_kv_t * kvobj, void * arg) {
	int * key_idx = (int *) arg;

	LOGD(kvobj->key << " : " << kvobj->val);

	*key_idx += 1;

	return 0;
}

int registerAllfunction(evhtp_request_t * req, void * args) {

	return 0;
}

void registercb(evhtp_request_t * req, void * args) {
	if (evhtp_request_get_method(req) != htp_method_GET) {
		evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
		return;
	}

	char* raw_query = (char*) req->uri->query_raw;
	evhtp_query_t * query;
	query = evhtp_parse_query_wflags(raw_query, strlen(raw_query), 0);
	std::string username = evhtp_kv_find(query, "username");
	std::string pasword = evhtp_kv_find(query, "password");

	// 查找用户是否已经注册
	{
		std::lock_guard<std::mutex> lock(user_table_lock);
		auto v = std::find_if(users_table.begin(), users_table.end(),
				[username](const User& user) -> bool {
					if (username.compare(user.uname) == 0) {
						return true;
					}
					return false;
				});

		if (v != users_table.end()) {
			rapidjson::Document d;
			d.SetObject();
			rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
			d.AddMember("code", 0, allocator);
			d.AddMember("what", "user exist", allocator);
			d.AddMember("uid", "-1", allocator);
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			d.Accept(writer);
			std::string result = buffer.GetString();
			evbuffer_add_printf(req->buffer_out, result.c_str());
			evhtp_send_reply(req, EVHTP_RES_OK);
			LOGD(result);

			return;
		}
	}

	// 注册用户
	char uid_buff[24];
	long long uid = 0;
	{
		std::lock_guard<std::mutex> lock(user_table_lock);
		uid = guid++;

		sprintf(uid_buff, "%lld", uid);

		User user;
		user.uid = uid_buff;
		user.uname = username;
		user.psd = pasword;
		users_table.push_back(user);

		std::string key("user:");
		key.append(uid_buff);
		syncRedis.setString(key, user.Serializable());
	}

	// 返回结果
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("code", 0, allocator);
	d.AddMember("what", "register success", allocator);
	d.AddMember("uid", uid_buff, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string result = buffer.GetString();
	evbuffer_add_printf(req->buffer_out, result.c_str());
	evhtp_send_reply(req, EVHTP_RES_OK);

	LOGD(result);
}

void logincb(evhtp_request_t * req, void * args) {
	if (evhtp_request_get_method(req) != htp_method_GET) {
		evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
		return;
	}

	char* raw_query = (char*) req->uri->query_raw;
	evhtp_query_t * query;
	query = evhtp_parse_query_wflags(raw_query, strlen(raw_query), 0);
	std::string userid = evhtp_kv_find(query, "userid");
	std::string username = evhtp_kv_find(query, "username");
	std::string password = evhtp_kv_find(query, "password");

	auto v = users.find(userid);
	if (v == users.end()) {
		rapidjson::Document d;
		d.SetObject();
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "user does not exist", allocator);
		d.AddMember("session", "", allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();
		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	if (v->second.uname.compare(username) != 0) {
		rapidjson::Document d;
		d.SetObject();
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "username is error", allocator);
		d.AddMember("session", "", allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();

		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	if (v->second.psd.compare(password) != 0) {
		rapidjson::Document d;
		d.SetObject();
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "psd is error", allocator);
		d.AddMember("session", "", allocator);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();

		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("code", 0, allocator);
	d.AddMember("what", "login success", allocator);
	d.AddMember("session", "1111111111", allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	std::string result = buffer.GetString();

	LOGD(result);

	evbuffer_add_printf(req->buffer_out, result.c_str());
	evhtp_send_reply(req, EVHTP_RES_OK);

	return;
}

void testcb(evhtp_request_t * req, void * args) {
//	const char* data = "{\"d\":\"ok\"}";
//	evbuffer_add_reference(req->buffer_out, data, strlen(data), NULL, NULL);
//	evhtp_send_reply(req, EVHTP_RES_OK);

	struct evbuffer * b = evbuffer_new();
	evhtp_send_reply_start(req, EVHTP_RES_OK);

	evbuffer_add(b, "foo", 3);
	evhtp_send_reply_body(req, b);
	evbuffer_add(b, "bar", 3);
	evhtp_send_reply_body(req, b);
	evhtp_send_reply_end(req);
	evbuffer_free(b);
}

void vh_testcb(evhtp_request_t * req, void * args) {
	const char* data = "{\"d\":\"ok\"}";
	evbuffer_add_reference(req->buffer_out, data, strlen(data), NULL, NULL);
	evhtp_send_reply(req, EVHTP_RES_OK);
}

void addcb(evhtp_request_t * req, void * args) {
	if (evhtp_request_get_method(req) != htp_method_GET) {
		evhtp_send_reply(req, EVHTP_RES_NOTFOUND);

		return;
	}

	evhtp_query_t * query;
	char* raw_query = (char*) req->uri->query_raw;
	query = evhtp_parse_query_wflags(raw_query, strlen(raw_query), 0);

	int key_idx = 0;
	evhtp_kvs_for_each(query, print_kvs, &key_idx);

	const char* a = evhtp_kv_find(query, "a");
	const char* b = evhtp_kv_find(query, "b");
	const char* c = evhtp_kv_find(query, "c");
	if (a == NULL) {
		return;
	}

	if (b == NULL) {
		return;
	}

	if (c == NULL) {
		return;
	}

	int ia = atoi(a);
	int ib = atoi(b);
	int ic = atoi(c);

	int result = ia + ib + ic;

	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("result", result, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string msg = buffer.GetString();

	LOGD(msg);

	evbuffer_add_printf(req->buffer_out, msg.c_str());
	evhtp_send_reply(req, EVHTP_RES_OK);
}
