#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>

int val=0;
void alarm_hand (int sig)
{
	signal(SIGALRM,alarm_hand);
	prf("got an alarm wakeup signal\n");
	val=5;
}
void DFS()
{
	while(val==0);
	printf("val equals %d\n",val);
}

int main (void)
{

	signal(SIGALRM,alarm_hand);
	alarm(10);
	printf("starting the search\n");
	DFS();
	printf("finished search\n");
	return 0;
}


