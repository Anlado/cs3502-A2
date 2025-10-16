#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_Sigint(int sig) {
    shutdown_flag = 1;
}

void handle_Sigusr1(int sig) {
    stats_flag = 1;
}

int main(int argc, char *argv[]) {
	printf("Consumer PID: %d\n", getpid());
	fflush(stdout);

	int max_lines = -1;
	int verbose = 0;
	int opt;

	while ((opt = getopt(argc, argv, "n:v")) != -1) {
        	switch (opt) {
			case 'n':
                		max_lines = atoi(optarg);
                		break;
			case 'v':
                		verbose = 1;
                		break;
        		default:
				fprintf(stderr, "Usage: %s [-n max_lines] [-v]\n", argv[0]);
                		exit(EXIT_FAILURE);
        	}
    	}

	struct sigaction sa_int, sa_usr1;

    	sa_int.sa_handler = handle_sigint;
    	sigemptyset(&sa_int.sa_mask);
    	sa_int.sa_flags = 0;
    	sigaction(SIGINT, &sa_int, NULL);

	sa_usr1.sa_handler = handle_sigusr1;
    	sigemptyset(&sa_usr1.sa_mask);
    	sa_usr1.sa_flags = 0;
    	sigaction(SIGUSR1, &sa_usr1, NULL);

    	int line_count = 0;
    	int char_count = 0;
    	char buffer[4096];

    	clock_t start_time = clock();

    	while (fgets(buffer, sizeof(buffer), stdin)) {
        	line_count++;
        	char_count += strlen(buffer);

        	if (verbose) {
            		fputs(buffer, stdout);
        	}

        	if (stats_flag) {
            		stats_flag = 0;
            		clock_t now = clock();
            		double elapsed = (double)(now - start_time) / CLOCKS_PER_SEC;

            		fprintf(stderr, "\n[Received SIGUSR1]\n");
            		fprintf(stderr, "Lines so far: %d\n", line_count);
            		fprintf(stderr, "Characters so far: %d\n", char_count);
            		fprintf(stderr, "Lines/sec: %.2f\n", line_count / elapsed);
            		fprintf(stderr, "Bytes/sec: %.2f\n", char_count / elapsed);
            		fprintf(stderr, "MB/sec: %.2f\n", (char_count / 1024.0 / 1024.0) / elapsed);
        	}

        	if (max_lines != -1 && line_count >= max_lines) {
            		break;
        	}

        	if (shutdown_flag) {
            		fprintf(stderr, "\n[Received SIGINT — shutting down gracefully]\n");
            		break;
        	}
    	}

    	clock_t end_time = clock();
    	double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    	fprintf(stderr, "\n[Final Statistics]\n");
    	fprintf(stderr, "Lines: %d\n", line_count);
    	fprintf(stderr, "Characters: %d\n", char_count);
    	fprintf(stderr, "Lines/sec: %.2f\n", line_count / total_time);
    	fprintf(stderr, "Bytes/sec: %.2f\n", char_count / total_time);
    	fprintf(stderr, "MB/sec: %.2f\n", (char_count / 1024.0 / 1024.0) / total_time);
    	fprintf(stderr, "Latency: %.2f\n", total_time);


    	return 0;

}



