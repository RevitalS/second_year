#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>

/* When a SIGUSR1 signal arrives, set this variable. */
int usr_interrupt = 0;

void signal_hand (int sig)
{
	usr_interrupt = 1;
}

/* The child process executes this function. */
void child_function (void)
{
	/* Perform initialization. */
	printf ("I'm here!!!  My pid is %d.\n", (int) getpid ());

	/* Let parent know you're done. */
	kill (getppid (), SIGUSR1);

	/* Continue with execution. */
	puts ("Bye, now....");
	exit (0);
}

int main (void)
{
	pid_t child_id;

	signal(SIGUSR1,signal_hand);

	/* Create the child process. */
	child_id = fork ();
	if (child_id == 0)
	 child_function ();          /* Does not return. */

	/* Busy wait for the child to send a signal. */
	while (!usr_interrupt);

	/* Now continue execution. */
	puts ("That's all, folks!");

	return 0;
}


