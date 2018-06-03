#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
int in_fd;
int counter=0;
int curr_write=0, curr_read=0;
void my_read()
{
	char buf[10];
	int count=0;
	//lseek(in_fd,curr_read,SEEK_SET);
	count=read(in_fd,buf,10);
	curr_read+=count;
	if(count!=0){
		buf[count-1]='\0';
		printf("I read now: %s, the num of bytes= %d\n",buf,count);
	}
	else
		printf("read nothing - just wasting CPU time\n");
}
void my_write()
{
	char buf[8]="hello";
	int count=0;
	buf[5]=(char)(counter+'0');
	buf[6]='\n';
	//lseek(in_fd,curr_write,SEEK_SET);
	count=write(in_fd,buf,7);
	curr_write+=count;
	counter++;
}

int main()
{
	pid_t child_id;
	in_fd=open("in.txt",O_RDWR |O_CREAT | O_TRUNC | O_SYNC,0666);
	child_id=fork();
	if(child_id>0)
	{	//father
		while(counter<5)
		{
			sleep(2);
			my_write();	
			
		}	
		close(in_fd);
		sleep(2);
		kill(child_id,SIGTERM);
		
	}else{
		while(1)
		{
			my_read();
		}
		
	}
	
}
