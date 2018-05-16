#include "example.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <fstream>

#include <evhtp/evhtp.h>

#include <log4z/log4z.h>
using namespace zsummer::log4z;

#include "data.h"
#include "bunissfunc.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filestream.h"

using namespace rapidjson;

bool load_config() {

	FILE *f;
	f = fopen("./config.cfg", "r");
	fseek(f, 0, SEEK_SET);

	Document root;
	FileStream inputStream(f);
	root.ParseStream<0>(inputStream);
	if (root.HasParseError()) {
		LOGE("fail parse json.");
		return -1;
	}

	if (!root.IsArray()) {
		LOGE("json is not object array.");
		return -1;
	}

	if (root.Empty()) {
		LOGE("json is empty.");
		return -1;
	}

	for (rapidjson::SizeType i = 0; i < root.Size(); i++) {
		User user;
		user.uid = root[i]["uid"].GetString();
		user.uname = root[i]["uname"].GetString();
		user.psd = root[i]["psd"].GetString();
		users.insert(std::make_pair(user.uid, user));
	}

	fclose(f);

	return true;
}

void thread_init_cb(evhtp_t * htp, evthr_t * thr, void * arg) {
	LOGD("thread_init_cb");
}

void thread_init_exit(evhtp_t * htp, evthr_t * thr, void * arg) {
	LOGD("thread_init_exit");
}

int server_main(int argc, char ** argv) {
	if (!load_config()) {
		return -1;
	}

	evbase_t * evbase = NULL;
	evhtp_t * htp = NULL;

	evhtp_callback_t * cb_1 = NULL;
	evhtp_callback_t * cb_2 = NULL;
	evhtp_callback_t * cb_3 = NULL;

	srand((unsigned) time(NULL));

	evbase = event_base_new();
	htp = evhtp_new(evbase, NULL);
	evhtp_use_threads_wexit(htp, thread_init_cb, thread_init_exit, 6, NULL);

//	evhtp_t * v1 = evhtp_new(evbase, NULL);
//	const char* vhost = "host1.com";
//	const char*path = "/host1";
//	evhtp_set_cb(v1, path, vh_testcb, (void*)vhost);
//	evhtp_add_vhost(htp, vhost, v1);

	cb_1 = evhtp_set_cb(htp, "/login", logincb, NULL);
	assert(cb_1 != NULL);

	cb_2 = evhtp_set_cb(htp, "/test", testcb, NULL);
	assert(cb_2 != NULL);

	cb_3 = evhtp_set_cb(htp, "/add", addcb, NULL);
	assert(cb_3 != NULL);

	int r = evhtp_bind_socket(htp, "0.0.0.0", 8006, 1024);
	if (r != 0) {

	}

	LOGD("running [port : " << 8006 << "]");

	r = event_base_dispatch(evbase);
	if (r != 0) {

	}

	evhtp_unbind_socket(htp);

	event_base_free(evbase);

	return 0;
}
