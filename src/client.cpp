//============================================================================
// Name        : evhtp_client.cpp
// Author      : lw
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "example.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>

#include <evhtp/evhtp.h>

#include <log4z/log4z.h>
using namespace zsummer::log4z;

static void request_cb(evhtp_request_t * req, void * arg) {
	LOGFMTA("hi %d\n", evbuffer_get_length(req->buffer_in));

}

int client_main(int argc, char ** argv) {
	evhtp_connection_t * conn;
	evhtp_request_t * request;
	evbase_t * evbase;

	evbase = event_base_new();

	clock_t t = clock();
	for (int i = 0; i < 1000; i++) {
		conn = evhtp_connection_new(evbase, "192.168.204.128", 8080);
		request = evhtp_request_new(request_cb, evbase);
		evhtp_make_request(conn, request, htp_method_GET, "/test");
	}

	clock_t t1 = clock();
	LOGFMTA("all exec times: %f", ((double) t1 - t) / CLOCKS_PER_SEC);

	event_base_dispatch(evbase);

	event_base_free(evbase);

	return 0;
}
