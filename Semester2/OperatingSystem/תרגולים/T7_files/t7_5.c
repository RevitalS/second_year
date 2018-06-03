#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#define MAXLINE 80
#define FIFONAME "myFifo"
int counter=0;

int main(void)
{
	int     n;
	int     fd;
	pid_t   pid;
	char    line[MAXLINE];
	
	//creating a FIFO
	if(mkfifo(FIFONAME,0666)<0) {
		printf("Unable to create a fifo");
		exit(-1);
	 }

	if ((pid = fork()) < 0) 
	{
		perror("fork error");
	} 
	else{ 
		if (pid > 0) 
		{ /* parent */
			int loopCount=0;
			//open fifo to write only
			fd=open(FIFONAME,O_WRONLY);
			printf("opened the FIFO for writing\n");
			while(loopCount<5)
			{
				char buf[8]="hello";
				buf[5]=(char)(counter+'0');
				write(fd,buf,6);
				counter++;
				loopCount++;
				sleep(2);
			}
			//close(fd);	//like pipes...
			sleep(10);	
		} 
		else
		{ /* child */
			int loopFlag=1;
			//father will continue when we open the pipe for reading
			sleep(5);
			//open fifo to read only
			fd=open(FIFONAME,O_RDONLY);
			while(loopFlag)
			{
				n = read(fd, line, MAXLINE);
				if(n==0)
					loopFlag=0;
				else{
					line[n]='\0';
					printf("I read now: %s, the num of bytes= %d\n",line,n);
				}
			}
			close(fd);
			//delete the pipe from the system
			unlink(FIFONAME);
		}
	}
	exit(0);
}

