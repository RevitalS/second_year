#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>



void signal_hand (int sig)
{
	signal(SIGUSR1,signal_hand);
	printf("got a SIGUSR1 signal\n");
	
}

void alarm_hand (int sig)
{
	signal(SIGALRM,alarm_hand);
	printf("got an alarm wakeup signal\n");
}


int main (void)
{
	int x=0;
	signal(SIGUSR1,signal_hand);
	signal(SIGALRM,alarm_hand);
	alarm(10);

	printf("waiting for SIGUSR1 or 10 seconds, my pid is: %d\n",getpid());
	pause();
	x=alarm(5);
	printf("woke up after %d seconds,waiting for SIGUSR1 or 5 seconds \n",10-x);
	pause();
	alarm(0);
	printf("I'm done\n");
	return 0;
}


