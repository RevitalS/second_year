#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
int glob=0;
void signal_hand (int sig)
{
	
	int i,fd;
	signal(SIGUSR1,signal_hand);
	printf("hello world SIGUSR1 %d\n",sig);
	glob++;
	for(i=0;i<1000000000;i++);
	printf("the value of glob is: %d\n",glob);
	fd=open("usr1.txt",O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IXGRP);
	write(fd,"hello\n",6);
	close(fd);
}

void signal_hand2 (int sig)
{

        int i,fd;
        signal(SIGUSR2,signal_hand2);
        printf("hello world SIGUSR2 %d\n",sig);
        glob++;   
        for(i=0;i<1000000000;i++);
        printf("the value of glob is: %d\n",glob);
        fd=open("usr2.txt",O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IXGRP);
        write(fd,"hello\n",6);
        close(fd);
}

     
int main (void)
{
       pid_t child_id;
       int status;
       signal(SIGUSR1,signal_hand);
	   signal(SIGUSR2,signal_hand2);
       /* Create the child process. */
       child_id = fork ();

	if (child_id > 0){ //father
		kill(child_id,SIGUSR1);
		//sleep(3);
		kill(child_id,SIGUSR2);	
		wait(&status);
		printf("father is done\n");
	}
     
	if (child_id == 0)
        {
	       while(glob<2);
		printf("son is done\n");
	}
     
        return 0;
}
