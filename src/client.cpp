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

static void request_cb(evhtp_request_t * req, void * arg) {
	printf("hi %zu\n", evbuffer_get_length(req->buffer_in));
}

int client_main(int argc, char ** argv) {
	evbase_t * evbase;
	evhtp_connection_t * conn;
	evhtp_request_t * request;

	evbase = event_base_new();

	for (int i = 0; i < 100; i++) {
		conn = evhtp_connection_new(evbase, "192.168.204.128", 8080);
		request = evhtp_request_new(request_cb, evbase);
		evhtp_make_request(conn, request, htp_method_GET, "/test");
	}

	event_base_loop(evbase, 0);
	event_base_free(evbase);

	return 0;
}
