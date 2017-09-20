#include "example.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <evhtp/evhtp.h>

#include <log4z/log4z.h>
using namespace zsummer::log4z;

/* evhtp_kvs_iterator */
int kvs_print(evhtp_kv_t * kvobj, void * arg) {
	int * key_idx = (int *) arg;

//	if (*key_idx) {
//		printf(", ");
//	}

	LOGFMTA("\"%s\": %s", kvobj->key, kvobj->val);

	*key_idx += 1;

	return 0;
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

	int key_idx = 0;
	char* raw_query = (char*) req->uri->query_raw;
	evhtp_query_t * query;
	query = evhtp_parse_query_wflags(raw_query, strlen(raw_query), 0);
	const char* a1 = evhtp_kv_find(query, "a");
	const char* b1 = evhtp_kv_find(query, "b");
	const char* c1 = evhtp_kv_find(query, "c");

	int a = 0;
	int b = 0;
	int c = 0;
	if (a1 != NULL) {
		a = atoi(a1);
	}

	if (b1 != NULL) {
		b = atoi(b1);
	}

	if (c1 != NULL) {
		c = atoi(c1);
	}

	evhtp_kvs_for_each(query, kvs_print, &key_idx);

	evbuffer_add_printf(req->buffer_out, "%d", a + b + c);
	evhtp_send_reply(req, EVHTP_RES_OK);
}

int server_main(int argc, char ** argv) {
	evbase_t * evbase = NULL;
	evhtp_t * htp = NULL;

	evhtp_callback_t * cb_1 = NULL;
	evhtp_callback_t * cb_2 = NULL;

	srand((unsigned) time(NULL));

	evbase = event_base_new();
	htp = evhtp_new(evbase, NULL);

//	evhtp_t * v1 = evhtp_new(evbase, NULL);
//	const char* vhost = "host1.com";
//	const char*path = "/host1";
//	evhtp_set_cb(v1, path, vh_testcb, (void*)vhost);
//	evhtp_add_vhost(htp, vhost, v1);

	cb_1 = evhtp_set_cb(htp, "/test", testcb, NULL);
	assert(cb_1 != NULL);

	cb_2 = evhtp_set_cb(htp, "/add", addcb, NULL);
	assert(cb_2 != NULL);

	int r = evhtp_bind_socket(htp, "0.0.0.0", 8080, 1024);
	if (r != 0) {

	}

	printf("http server running [%d]\n", 8080);

	r = event_base_dispatch(evbase);
	if (r != 0) {

	}
	evhtp_unbind_socket(htp);

	event_base_free(evbase);

	return 0;
}
