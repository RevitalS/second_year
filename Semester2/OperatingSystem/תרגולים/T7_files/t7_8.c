//server side

#include<string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

int main()
{
	int fd_read,fd_write,choice;
	char buf[80];
	fd_read=open("fifo_clientTOserver",O_RDONLY);
	printf("waiting for client request\n");

	read(fd_read,&choice,sizeof(int));

	sleep(5);
	fd_write=open("fifo_serverTOclient",O_WRONLY);	

	switch(choice) {

		case 1: 
		 strcpy(buf,"Linux");
		 write(fd_write,buf,6*sizeof(char)); 
		 printf("\n Data sent to client \n");
		 break;
		case 2:

		 strcpy(buf,"Ubuntu");
		 write(fd_write,buf,7*sizeof(char)); 
		 printf("\nData sent to client\n");
		 break;
		case 3: 
		 strcpy(buf,"2.6.32");
		 write(fd_write,buf,7*sizeof(char)); 
		 printf("\nData sent to client\n");
	}

	close(fd_write);
 	close(fd_read);
}
