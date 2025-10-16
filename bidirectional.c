#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int pipe1[2];
    int pipe2[2];
    char buffer[BUFFER_SIZE];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

        close(pipe1[1]);
        close(pipe2[0]);

        read(pipe1[0], buffer, BUFFER_SIZE);
        printf("Child received: %s\n", buffer);

        char child_response[] = "hello parent, I love artificial sweeteners";
        write(pipe2[1], child_response, strlen(child_response) + 1);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);

    } else {
        close(pipe1[0]);
        close(pipe2[1]);

        char parent_message[] = "Hello child, I love processed foods";
        write(pipe1[1], parent_message, strlen(parent_message) + 1);

        read(pipe2[0], buffer, BUFFER_SIZE);
        printf("Parent received: %s\n", buffer);

        close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL);
    }

    return 0;
}
