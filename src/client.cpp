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

static void add_cb(evhtp_request_t * req, void * arg) {
	char buf[1024];
	struct evbuffer *evbuf = req->buffer_in;
	ev_ssize_t len = evbuffer_get_length(evbuf);
	memcpy(buf, evbuffer_pullup(evbuf, len), len);
	buf[len] = '\0';
	LOGFMTA("%s", buf);
	evbuffer_drain(evbuf, len);
}

int main_client(int argc, char ** argv) {

	evhtp_connection_t * conn;
	evhtp_request_t * request;
	evbase_t * evbase;

	evbase = event_base_new();

	{
		conn = evhtp_connection_new(evbase, "192.168.204.128", 8080);
		request = evhtp_request_new(add_cb, evbase);
		evhtp_make_request(conn, request, htp_method_GET,
				"/add?a=111&b=222&c=333");
	}

	event_base_dispatch(evbase);
	event_base_free(evbase);

	return 0;
}
