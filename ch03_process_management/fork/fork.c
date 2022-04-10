#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

static void child()
{
	printf("I'm child! my pid is %d.\n", getpid());
	exit(EXIT_SUCCESS);
}

static void parent(pid_t pid_c)
{
	printf("I'm parent! my pid is %d.\n", getpid());
	printf("\tand, my child's pid is %d.\n", pid_c);
	exit(EXIT_SUCCESS);
}

int main(void)
{
	pid_t ret;
	ret = fork();

	if ( ret == -1 ) {
		err(EXIT_FAILURE, "failed to fork.");
	}

	if ( ret == 0 ) {
		/*
		* child process comes here because
		* fork() returns 0 for child process
		*/
		printf("%d\n", ret);
		child();
	} else {
		/*
		* parent process comes here because
		* fork() returns the pid of newly created child process
		*/
		printf("%d\n", ret);
		parent(ret);
	}

	err(EXIT_FAILURE, "shouldn't reach here");
}