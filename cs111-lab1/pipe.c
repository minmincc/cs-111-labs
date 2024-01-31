#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void close_pipes(int pipes[], int num_pipes) {
    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <command> [<command> ...]\n", argv[0]);
        exit(EINVAL);
    }

    if (argc == 2) {
        execlp(argv[1], argv[1], NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    int num_pipes = argc - 2;
    int pipefds[2 * num_pipes];

    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < argc - 1; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (i != 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            if (i != argc - 2) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            close_pipes(pipefds, 2 * num_pipes);
            execlp(argv[i + 1], argv[i + 1], NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }

    close_pipes(pipefds, 2 * num_pipes);
    while (wait(NULL) > 0);
    return 0;
}