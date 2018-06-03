#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXLINE 80
int counter=0;

int main(void)
{
	int     n;
	int     fd[2];
	pid_t   pid;
	char    line[MAXLINE];

	if (pipe(fd) < 0)
		perror("pipe error");
	if ((pid = fork()) < 0) 
	{
		perror("fork error");
	} 
	else{ 
		if (pid > 0) 
		{ /* parent */
			int loopCount=0;
			close(fd[0]);
			while(loopCount<5)
			{
				char buf[8]="hello";
				buf[5]=(char)(counter+'0');
				write(fd[1],buf,6);
				counter++;
				loopCount++;
				sleep(2);
			}
			//close(fd[1]);	//very important, why?
			sleep(10);	
		} 
		else
		{ /* child */
			int loopFlag=1;
			close(fd[1]);
			while(loopFlag)
			{
				n = read(fd[0], line, MAXLINE);
				if(n==0)
					loopFlag=0;
				else{
					line[n]='\0';
					printf("I read now: %s, the num of bytes= %d\n",line,n);
				}
			}
			close(fd[0]);
		}
	}
	exit(0);
}

