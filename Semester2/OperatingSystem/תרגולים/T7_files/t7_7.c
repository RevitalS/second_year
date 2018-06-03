//client side

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

int main()
{
	int fd_read,fd_write,choice;
	char buf[80];
	fd_write=open("fifo_clientTOserver",O_WRONLY);

	printf("Choose the request to be sent to server from options below");
 	printf("\n\t\t Enter 1 for O.S.Name \n \
           	Enter 2 for Distribution \n \
                Enter 3 for Kernel version \n");
 	scanf("%d",&choice);
	write(fd_write,&choice,sizeof(int));
	fd_read=open("fifo_serverTOclient",O_RDONLY);
	

 	read (fd_read,buf,80);
 	printf("\n ***Reply from server is %s***\n",buf);
	close(fd_write);
 	close(fd_read);
}
