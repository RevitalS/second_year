#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
int main()
{
	int file;

	file = mkfifo("fifo_clientTOserver",0666); 
	if(file<0) {
		 printf("Unable to create a fifo");
		 exit(-1);
	 }

	file = mkfifo("fifo_serverTOclient",0666); 
	if(file<0) {
		 printf("Unable to create a fifo");
		 exit(-1);
	 }
	exit(1);
}
