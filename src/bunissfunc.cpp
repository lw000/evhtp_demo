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

#include <log4z/log4z.h>
using namespace zsummer::log4z;

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filestream.h"

using namespace rapidjson;


/* evhtp_kvs_iterator */
static int kvs_print(evhtp_kv_t * kvobj, void * arg) {
	int * key_idx = (int *) arg;

	LOGD(kvobj->key << " : " << kvobj->val);

	*key_idx += 1;

	return 0;
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
	std::string pasword = evhtp_kv_find(query, "pasword");

	auto v = users.find(userid);
	if (v == users.end()) {
		Document d;
		d.SetObject();
		Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "user does not exist", allocator);
		d.AddMember("session", "", allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();
		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	if (v->second.uname.compare(username) != 0) {
		Document d;
		d.SetObject();
		Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "username is error", allocator);
		d.AddMember("session", "", allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();

		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	if (v->second.psd.compare(pasword) != 0) {
		Document d;
		d.SetObject();
		Document::AllocatorType& allocator = d.GetAllocator();
		d.AddMember("code", -1, allocator);
		d.AddMember("what", "psd is error", allocator);
		d.AddMember("session", "", allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);

		std::string result = buffer.GetString();

		LOGD(result);

		evbuffer_add_printf(req->buffer_out, result.c_str());
		evhtp_send_reply(req, EVHTP_RES_OK);

		return;
	}

	Document d;
	d.SetObject();
	Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("code", 0, allocator);
	d.AddMember("what", "login success", allocator);
	d.AddMember("session", "1111111111", allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
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
	evhtp_kvs_for_each(query, kvs_print, &key_idx);

	const char* a = evhtp_kv_find(query, "a");
	const char* b = evhtp_kv_find(query, "b");
	const char* c = evhtp_kv_find(query, "c");
	if (a == NULL) {
		return;
	}

	if (b != NULL) {
		return;
	}

	if (c != NULL) {
		return;
	}

	int ia = atoi(a);
	int ib = atoi(b);
	int ic = atoi(c);

	int result = ia + ib + ic;

	Document d;
	d.SetObject();
	Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("a", ia, allocator);
	d.AddMember("b", ib, allocator);
	d.AddMember("c", ic, allocator);
	d.AddMember("result", result, allocator);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string msg = buffer.GetString();

	LOGD(msg);

	evbuffer_add_printf(req->buffer_out, msg.c_str());
	evhtp_send_reply(req, EVHTP_RES_OK);
}
