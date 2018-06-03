#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>



void signal_hand (int sig)
{
	signal(SIGUSR1,signal_hand);
	printf("got a signal\n");
}


int main (void)
{

	signal(SIGUSR1,signal_hand);

	printf("waiting for SIGUSR1\n");
	pause();
	printf("waiting for SIGUSR1 again\n");
	pause();
	printf("I'm done\n");
	return 0;
}


