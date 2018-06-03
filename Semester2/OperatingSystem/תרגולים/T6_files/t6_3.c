#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void sighandler(int signum, siginfo_t *info, void *ptr)
{
    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n",(unsigned long)info->si_pid);
}

int main()
{
	struct sigaction act;
	sigset_t block_mask;
	printf("I am %lu \n", (unsigned long)getpid());
	sigfillset (&block_mask);
	act.sa_mask = block_mask;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = sighandler;

	sigaction(SIGTERM, &act, NULL);

	sleep(60);
	printf("done\n");

    return 0;
}
