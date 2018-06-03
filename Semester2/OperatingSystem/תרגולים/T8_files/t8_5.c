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
	struct shmid_ds ds;

	if ((key = ftok("shmdemo.c", 'R')) == -1) {
		perror("ftok");
		exit(1);
	}

	/* grab the shared memory created by t8_3.c: */
	if ((shmid = shmget(key, SHM_SIZE, 0)) == -1) {
		perror("shmget");
		exit(1);
	}

	/* get info: */
	if (shmctl(shmid,IPC_STAT,&ds) == -1) {
		perror("shmctl");
		exit(1);
	}else{
		printf("owner uid/gid: %d/%d\n",ds.shm_perm.uid,ds.shm_perm.gid);
        printf("segment size: %d\n",ds.shm_segsz);
		printf("num of attached: %d\n",ds.shm_nattch);
	}

	return 0;
}

