#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // If no command is passed, exit with EINVAL
        fprintf(stderr, "Usage: %s <command> [<command> ...]\n", argv[0]);
        return EINVAL;
    }

    if (argc == 2) { // If only one command is passed, execute it directly
        execlp(argv[1], argv[1], NULL);
        perror("execlp");
        return EXIT_FAILURE;
    }

    int pipefds[2 * (argc - 2)]; // Array to hold the file descriptors for pipes
    int pid;

    // Create pipes
    for (int i = 0; i < (argc - 2); i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Execute each command
    for (int i = 0; i < argc - 1; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], 0); // Set up read end of pipe
            }

            if (i < argc - 2) {
                dup2(pipefds[i * 2 + 1], 1); // Set up write end of pipe
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (argc - 2); j++) {
                close(pipefds[j]);
            }

            execlp(argv[i + 1], argv[i + 1], NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe file descriptors in parent
    for (int i = 0; i < 2 * (argc - 2); i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes
    while ((pid = wait(NULL)) != -1);

    return 0;
}