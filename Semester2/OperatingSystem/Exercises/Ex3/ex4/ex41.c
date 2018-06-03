/*********************************
name: Shalev Maman
id: 313514143
group: 05
ex: ex41.
**********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define ERR -1
#define STDOUT 1
#define STDIN 0

#define FLAGS IPC_CREAT | 0644
#define EXIT _exit(-1)
#define SHM_SIZE 1024


#define TRUE 1
#define FALSE 0

#define ASKFORINPUT "Please enter request code\n"



key_t key,semSerKey,semClientKey;
char job;
char *addr;
int shmid,serSemid,clieSemid;
struct sigaction usr_action;
struct sembuf sopServer[1];
struct sembuf sopClient[1];


union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

void finish(int shmid) {
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		write(ERR,"ERROR In shmctl\n",15);
		EXIT;
	}
	shmdt ( addr );
}

char getParams() {
	char job[2];
	read(STDIN,job,2);
	return job[0];
}

void initSystem() {
	key = ftok("result.txt", 'H');
	if (key < 0) {
		write(ERR,"ERROR In ftok\n",14);
		EXIT;
	}

	semClientKey = ftok("result.txt", 'C');
	if (key < 0) {
		write(ERR,"ERROR In ftok\n",14);
		EXIT;
	}

	semSerKey = ftok("result.txt", 'S');
	if (key < 0) {
		write(ERR,"ERROR In ftok\n",14);
		EXIT;
	}

	shmid = shmget(key,SHM_SIZE,FLAGS);

	if (shmid < 0) {
		write(ERR,"ERROR In SHMID\n",15);
		EXIT;
	}
	addr = (char *)shmat ( shmid ,NULL,0 );
	if (addr == (char*)-1) {
		write(ERR,"ERROR In shmat\n",15);
		EXIT;
	}
	
	serSemid = semget(semSerKey, 1, IPC_CREAT | 0666);
	clieSemid = semget(semClientKey, 1, IPC_CREAT | 0666);

}

void getSignals(int sig) {
	initSystem();
	usr_action.sa_handler = getSignals;
	sigaction (SIGCHLD, &usr_action, NULL);
}

void initSignals() {
	sigset_t block_mask;
	sigfillset(&block_mask);
	//The end function.
	usr_action.sa_handler = getSignals;
	usr_action.sa_mask = block_mask;
	usr_action.sa_flags = 0;
	//Connect function to signal.
	sigaction (SIGCHLD, &usr_action, NULL);
}

int main(void) {

	union semun semarg;
	initSignals();
	initSystem();


	while (TRUE) {
		write(STDOUT,ASKFORINPUT,strlen(ASKFORINPUT));
		job = getParams();
		if (job == '9') { break; }
		//lock client
		sopClient->sem_op = -1;
        semop ( clieSemid , sopClient , 1 );
        printf("In client\n");
		addr[0] = job;
		printf("In client\n");
		//unlock server
		job = getParams();
		sopServer->sem_op = 1;
		semop ( serSemid , sopServer , 1 );
	}
	finish(shmid);

	return TRUE;
}