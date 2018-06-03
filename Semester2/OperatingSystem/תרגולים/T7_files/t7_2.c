#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
int in_fd;
int counter=0;
int curr_write=0, curr_read=0;
void sig_read(int num)
{
	char buf[10];
	int count;
	lseek(in_fd,curr_read,SEEK_SET);
	count=read(in_fd,buf,10);
	curr_read+=count;
	buf[count-1]='\0';
	printf("I read now: %s, the num of bytes= %d\n",buf,count);
	signal(SIGUSR1,sig_read);	
}

void sig_write(int num)
{
	char buf[8]="hello";
	int count=0;
	buf[5]=(char)(counter+'0');
	buf[6]='\n';
	lseek(in_fd,curr_write,SEEK_SET);
	count=write(in_fd,buf,7);
	curr_write+=count;
	counter++;
	signal(SIGUSR2,sig_write);
}

int main()
{
	pid_t child_id;
	in_fd=open("in.txt",O_RDWR |O_CREAT | O_TRUNC,0666);
	signal(SIGUSR1,sig_read);
	signal(SIGUSR2,sig_write);
	child_id=fork();
	if(child_id>0)
	{	//father
		while(counter<5)
		{
			pause();
			sleep(2);	
			kill(child_id,SIGUSR1);
		}	
		close(in_fd);
		sleep(2);
		kill(child_id,SIGTERM);
		
	}else{
		while(1)
		{
			kill(getppid(),SIGUSR2);
			pause();
		}
		
	}
	
}
