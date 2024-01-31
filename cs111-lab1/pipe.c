#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        // If no command is passed, exit with EINVAL
        fprintf(stderr, "Usage: %s <command> [<command> ...]\n", argv[0]);
        exit(EINVAL);
    }

    if (argc == 2) {
        // Execute directly if only one command is passed
        pid_t cpid = fork();
        if (cpid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {
            // Child process
            if (execlp(argv[1], argv[1], NULL) < 0) {
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process waits for child to finish
            int status;
            waitpid(cpid, &status, 0);
            exit(WEXITSTATUS(status));
        }
    } else {
        // Handle multiple commands
        int fds[2];
        int prev_fd = -1; // File descriptor for the read end of the previous pipe

        for (int i = 1; i < argc; ++i) {
            if (i < argc - 1) {
                // Create a pipe for each pair of commands
                if (pipe(fds) == -1) {
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
                    close(fds[0]); // Close unused read end
                    dup2(fds[1], STDOUT_FILENO);
                    close(fds[1]); // Close write end after duplicating
                }
                execlp(argv[i], argv[i], NULL);
                // Only reached if execlp fails
                perror("execlp");
                exit(EXIT_FAILURE);
            } else {
                // Parent process
                if (prev_fd != -1) {
                    close(prev_fd);
                }
                if (i < argc - 1) {
                    close(fds[1]); // Close unused write end
                    prev_fd = fds[0]; // Save read end for next iteration
                }

                int status;
                waitpid(pid, &status, 0);
                if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                    exit(EXIT_FAILURE); // Exit if child process fails
                }
            }
        }
    }

    return 0;
}