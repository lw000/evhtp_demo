#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "example.h"

int main(int argc, char ** argv) {

	std::string s(argv[1]);

	if (s.compare("s") == 0) {
		server_main(argc, argv);
	} else if (s.compare("c") == 0) {
		client_main(argc, argv);
	} else {
		printf("error. \n");
	}

	while (1) {
		sleep(1);
	}

	return 0;
}
