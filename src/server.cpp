#include "example.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <evhtp/evhtp.h>

void testcb(evhtp_request_t * req, void * a) {
	const char* data = "{\"d\":\"ok\"}";
	evbuffer_add_reference(req->buffer_out, data, strlen(data), NULL, NULL);
	evhtp_send_reply(req, EVHTP_RES_OK);
}

void addcb(evhtp_request_t * req, void * a) {

	if (evhtp_request_get_method(req) != htp_method_GET) {
		evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
		return;
	}

	struct evbuffer * b = evbuffer_new();

	evhtp_send_reply_start(req, EVHTP_RES_OK);

	evbuffer_add(b, "foo", 3);
	evhtp_send_reply_body(req, b);
	evhtp_send_reply_end(req);

	evbuffer_free(b);
}

int server_main(int argc, char ** argv) {
	evbase_t * evbase = NULL;
	evhtp_t * htp = NULL;

	evhtp_callback_t * cb_1 = NULL;
	evhtp_callback_t * cb_2 = NULL;

	srand((unsigned) time(NULL));

	evbase = event_base_new();
	htp = evhtp_new(evbase, NULL);

	cb_1 = evhtp_set_cb(htp, "/test", testcb, NULL);
	assert(cb_1 != NULL);

	cb_2 = evhtp_set_cb(htp, "/add", addcb, NULL);
	assert(cb_2 != NULL);

	int r = evhtp_bind_socket(htp, "0.0.0.0", 8080, 1024);
	if (r != 0) {

	}

	printf("http server running [%d]\n", 8080);

	r = event_base_loop(evbase, 0);
	if (r != 0) {

	}

	return 0;
}
