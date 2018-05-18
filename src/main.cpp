#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "example.h"

#include <log4z/log4z.h>
using namespace zsummer::log4z;

int main(int argc, char ** argv) {

	ILog4zManager::getInstance()->start();

	std::string s(argv[1]);

	if (s.compare("s") == 0) {
		main_server(argc, argv);
	} else if (s.compare("c") == 0) {
		main_client(argc, argv);
	} else {
		printf("error. \n");
	}

	while (1) {
		sleep(1);
	}

	return 0;
}
