#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
int fda[2];
int counter=0;

void sig_read(int num)
{
	char buf[10];
	int count;
	count=read(fda[0],buf,10);
	buf[count]='\0';
	printf("I read now: %s, the num of bytes= %d\n",buf,count);
	signal(SIGUSR1,sig_read);	
}

void sig_write(int num)
{
	char buf[8]="hello";
	int count=0;
	buf[5]=(char)(counter+'0');
	count=write(fda[1],buf,6);
	counter++;
	signal(SIGUSR2,sig_write);
}

int main()
{
	pid_t child_id;
	pipe(fda);
	signal(SIGUSR1,sig_read);
	signal(SIGUSR2,sig_write);
	child_id=fork();
	if(child_id>0)
	{	//father
		close(fda[0]);	//close read side
		while(counter<5)
		{
			pause();
			sleep(2);	
			kill(child_id,SIGUSR1);
		}	
		close(fda[1]);
		kill(child_id,SIGTERM);
		
	}else{
		close(fda[1]);	//close write side
		while(1)
		{
			kill(getppid(),SIGUSR2);
			pause();
		}
		close(fd[0]);
	}
	
}
