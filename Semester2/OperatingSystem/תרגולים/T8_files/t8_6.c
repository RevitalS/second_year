#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

int main(void)
{
	key_t key;
	int shmid;

	if ((key = ftok("shmdemo.c", 'R')) == -1) {
		perror("ftok");
		exit(1);
	}

	/* grab the shared memory created by t8_3.c: */
	if ((shmid = shmget(key, SHM_SIZE, 0)) == -1) {
		perror("shmget");
		exit(1);
	}

	/* remove it: */
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	}

	return 0;
}

