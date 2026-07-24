#include <stdio.h>
#include "../include/parser.h"


int main(int argc, char** argv) {
	if(argc < 2) {
		printf("No arguments specified\n");
		return -1;
	}

	char* file = argv[1];

    int status = parse(file);

    if(status < 0) {
        perror("Error while parsing file\n");
        return -1;
    }

	return 0;
}
