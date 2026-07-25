#include <stdio.h>
#include "../include/parser.h"


int main(int argc, char** argv) {

	const char* outputfile = "out.txt"; 

	if(argc < 2) {
		printf("No arguments specified\n");
		return -1;
	}

	const char* inputfile = argv[1];

    int status = parse(inputfile, outputfile);

    if(status < 0) {
        perror("Error while parsing file\n");
        return -1;
    }

	printf("\nFile parsed successfully!");

	return 0;
}
