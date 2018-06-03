#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#define SHM_SIZE 4096
#define FLAGS IPC_CREAT | 0644

int main()
{
	int shm_id;
	shm_id=shmget(IPC_PRIVATE,SHM_SIZE,FLAGS);
	if(shm_id==-1)
	{
		printf("error creating shared memory\n");
		exit(0);
	}
	printf("the shared memory segment ID is: %d\n",shm_id);
}
