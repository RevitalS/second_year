#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <stdio.h>

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int semid;
union semun semarg;

main()
{
    int err;
    struct sembuf sops[1];

    semid=semget ( IPC_PRIVATE , 1 , 0600 );
    
    semarg.val=1;
    semctl ( semid , 0 , SETVAL , semarg );
    sops->sem_num = 0;
    sops->sem_flg = 0;	
    if ( fork()==0 ) {
        sops->sem_op = -1;
        semop ( semid , sops , 1 );
        printf("son got the lock\n");
		sleep( 5 );	//critical section actions
		printf("son releases the lock\n");
		sops->sem_op = 1;
        semop ( semid , sops , 1 );
		sleep(6);
		printf("delete the semaphore from son\n");
        err=semctl ( semid , 0 , IPC_RMID , semarg );
		if(err==-1)
		{
			printf("son delete semaphore error\n");
		}
    }
    
	else {
        sops->sem_op = -1;
        semop ( semid , sops , 1 );
        printf("father got the lock\n");
		sleep( 5 );	//critical section actions
		printf("father releases the lock\n");
		sops->sem_op = 1;
        semop ( semid , sops , 1 );
		sleep(6);
		printf("delete the semaphore from father\n");
        err=semctl ( semid , 0 , IPC_RMID , semarg );
		if(err==-1)
		{
			printf("father delete semaphore error\n");
		}
    }
}


