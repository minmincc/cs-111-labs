#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc <= 1) {
		// Invalid argument (less than one)
		exit(EINVAL);
	}
	if (argc == 2) {
		// Execute directly if only one argument
		pid_t cpid = fork();
		if (cpid < 0) {
			perror("Fork failed");
			exit(EXIT_FAILURE);
		}
		if (cpid == 0) {
			if (execlp(argv[1], argv[1], NULL) < 0) {
				perror("Executing command failed");
        		exit(EXIT_FAILURE);
			}
		}
		else {
			int status = 0;
			waitpid(cpid, &status, 0);
			exit(WEXITSTATUS(status));
		}
	}
	else {
		// More than two arguments
		int fds[2];
		int prevEnd = -1;

		for (int i = 1; i < argc; i++) {

			if (pipe(fds) < 0) {
				perror("Pipe creation failed");
				exit(EXIT_FAILURE);
			}

			int pid = fork();
			if (pid < 0) {
				perror("Fork failed");
				exit(EXIT_FAILURE);
			} 
			else if (pid == 0) {
				// Child process
				if (prevEnd != -1) {
					// Redirect stdin to the read end of previous pipe
					dup2(prevEnd, STDIN_FILENO);
					close(prevEnd);
				}

				// If not the last command
				if (i < argc - 1) {
					// Redirect stdout to write end of current pipe
					dup2(fds[1], STDOUT_FILENO);
				}

				close(fds[0]);
				close(fds[1]);

				// Execute the current command
				execlp(argv[i], argv[i], NULL);
				exit(EXIT_FAILURE);
			} 
			else {
				// Parent process
				close(fds[1]);
				prevEnd = fds[0];

				// Wait for child process
				int status = 0;
				waitpid(pid, &status, 0);

				if (!WIFEXITED(status)) {
					// Child process terminate failed
					exit(EXIT_FAILURE);
				}

				int exitStatus = WEXITSTATUS(status);
				if (exitStatus != 0) {
					// Child process returned an error
					exit(exitStatus);
				}
			}
		}
	}
	return 0;
}
