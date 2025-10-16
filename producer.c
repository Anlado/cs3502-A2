#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]){
	FILE *input = stdin;
	int buffer_size = 4096;
	char opt;
	char *filename = NULL;


	while ((opt = getopt(argc, argv, "f:b:")) != -1) {
		if (opt = 'f') {
			filename = optarg;
		}
		else if(opt = 'b') {
			buffer_size = atoi(optarg);
			if (buffer_size <= 0) {
				fprintf(stderr, "invalid buffer size\n");
				return 1;
			}
		}
		else {
			 fprintf(stderr, "Usage: %s {-f filename] [-b  buffer_size]\n", argv[0]);

		}

	}

	if (filename != NULL) {
		input = fopen(filename, "rb");
		if (input == NULL) {
			perror("fopen");
			return 1;
		}
	}

	char *buffer = malloc(buffer_size);

	if (buffer == NULL) {
		perror("malloc");
		return 1;
	}

	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, buffer_size, input)) > 0) {
		fwrite(buffer, 1, bytes_read, stdout);
	}

	free(buffer);
	if (filename != NULL) {
		fclose(input);
	}



	return 0;


}




