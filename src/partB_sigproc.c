#include <stdio.h>
#include <stdlib.h>

volatile static sig_atomic_t usr1Happened = 0;

void sigusr_handler() {
	usr1Happened = 1;
}

int main(void) {
	int numChildren;
	pid_t pid;

	struct sigaction sa;
	sa.sa_handler = sigusr_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);

	printf("Enter the number of children: ");
	scanf("%d", &numChildren);

	printf("PID = %d : Parent running...\n", getpid());
	for(int i = 0; i < numChildren; i++) {
		if((pid = fork()) < 0) {
			perror("fork");
			exit(1);
		}
		else if(pid == 0) {
			// child
			printf("PID = %d : Child running...\n", getpid());
			while(!usr1Happened) {
				if(getppid() == 1) {
					exit(1);
				}
			}

			printf("PID = %d : Child received USR1. \n", getpid());
			printf("PID = %d : Child exiting.\n", getpid());
			exit(EXIT_SUCCESS);
		} else if (pid > 0) {
			// parent
			continue;
		}
	}

	while(wait(NULL) > 0);

	printf("PID = %d : Children finished, parent exiting.\n", getpid());
	return EXIT_SUCCESS;
}
