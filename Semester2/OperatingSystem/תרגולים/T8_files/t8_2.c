#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHM_ID 720911

int main()
{
	char* shmaddr;
	if((shmaddr=shmat(SHM_ID,0,0))==(char*)-1)
	{
		printf("error in attaching to the shared memory\n");
		exit(0);
	}
	printf("the shared memory contains: %s\n",shmaddr);
	strcpy(shmaddr,"hello world");

	if(shmdt(shmaddr)==-1)
		printf("detaching error\n");
}
