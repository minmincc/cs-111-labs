#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void executeCommand(const char* cmd) {
    if (execlp(cmd, cmd, NULL) < 0) {
        perror("execlp");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <command> [<command> ...]\n", argv[0]);
        exit(EINVAL);
    }

    int pipe_fds[2]; // For pipe file descriptors
    int prev_fd = -1; // For the read end of the previous pipe

    for (int i = 1; i < argc; i++) {
        if (i < argc - 1) {
            if (pipe(pipe_fds) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (i < argc - 1) {
                close(pipe_fds[0]); // Close the read end of the pipe
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]); // Close the write end of the pipe
            }

            executeCommand(argv[i]);
        } else {
            // Parent process
            if (prev_fd != -1) {
                close(prev_fd);
            }

            if (i < argc - 1) {
                close(pipe_fds[1]); // Close the write end of the pipe
                prev_fd = pipe_fds[0]; // Set up for the next command
            }

            int status;
            waitpid(pid, &status, 0);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                exit(EXIT_FAILURE); // Exit if a child process fails
            }
        }
    }

    return 0;
}