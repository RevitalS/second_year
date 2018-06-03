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
int x=2;
void sigHandler(int sig)
{
	x=-1
}
main()
{
    int err;
	pid_t pid;
    struct sembuf sops[1];

    semid=semget ( IPC_PRIVATE , 1 , 0600 );
    
    semarg.val=1;
    semctl ( semid , 0 , SETVAL , semarg );
    sops->sem_num = 0;
    sops->sem_flg = 0;
	pid=fork();	
    if ( pid==0 ) {
		//signal(SIGUSR1,sigHandler)
		//sops->sem_flg = SEM_UNDO;
        sops->sem_op = -1;
        semop ( semid , sops , 1 );
        printf("son got the lock\n");
		while(x>0)
		{
			sleep(1);
			printf("son got the lock\n");
		}
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
	else {//father
		sleep(1);
		kill(pid,SIGUSR1);
		printf("father sent a signal to the son pid: %d\n",pid);
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


