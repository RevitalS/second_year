/*********************************
name: Shalev Maman
id: 313514143
group: 05
ex: ex42.
**********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>

#define ERR -1
#define STDOUT 1
#define TRUE 1
#define FALSE 0

#define FLAGS IPC_CREAT | 0644
#define SHM_SIZE 1024
#define FILE_NAME "result.txt"

#define EXIT _exit(-1)


union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};


struct JobNode {
	struct JobNode* next;
	char task;
};
typedef struct JobNode JobNode;

pthread_t th[5];
union semun semargServ;
union semun semargClient;
key_t key,semSerKey,semClientKey;
char job;
pthread_mutex_t lock,countLock,writeLock;
char *addr;
int shmid,serSemid,clieSemid;
struct sigaction usr_action;
struct sembuf sopServer[1];
struct sembuf sopClient[1];
int resultFD, internal_count;
JobNode* first;
JobNode* end;

void initCounter() {
	internal_count = 0;
}


void initQueue(void) {
	first = (JobNode*)malloc(sizeof(JobNode));
	end = first;
	first->next = NULL;
}

void addNode(char val) {
	JobNode* temp = (JobNode*)malloc(sizeof(JobNode));
	end->next = temp;
	temp->next = NULL;
	if (first == end) {
		first->task = val;
	} else {
	    end->task = val;
	}

	end = temp;
}

void createSema(void) {
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
	serSemid = semget(semSerKey, 1, IPC_CREAT | 0666);
	clieSemid = semget(semClientKey, 1, IPC_CREAT | 0666);
    semargServ.val = 1;
    semctl ( serSemid , 0 , SETVAL , semargServ );
    sopServer->sem_op = 1;
    sopServer->sem_num = 0;
    sopServer->sem_flg = 0;
    semctl ( clieSemid , 0 , SETVAL , semargServ );
    sopServer->sem_num = 0;
    sopServer->sem_flg = 0;

}


void takeNodeOut(void) {
	if (first == end) {return;}
	JobNode* temp = first;
	first = first->next;
	free(temp);
}

void addToCount(int value) {
	    pthread_mutex_lock(&lock);
	    internal_count += value;
        pthread_mutex_unlock(&lock);
}
void writeToResults(void) {
	pthread_mutex_lock(&writeLock);
	int myTid = pthread_self();
	char str[16];
	sprintf(str, "%d", myTid);
	write(resultFD,str,strlen(str));
	write(resultFD,",",1);
	sprintf(str, "%d", myTid);
	write(resultFD,str,strlen(str));
	write(resultFD,"\n",2);
	pthread_mutex_unlock(&writeLock);
}


void handleJob(char val) {
	switch(val) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			addToCount(val - '0');
			break;
		case '6':
			writeToResults();
			break;
		default:
			break;
	}
}

void* dealWithPick(void* arg) {
	char val = '0';
	while (TRUE) {
		while(first == end);
		if (pthread_mutex_trylock(&lock) == 0)
		{
			if (first == end) { continue; }
        	val = first->task;
        	takeNodeOut();
        	pthread_mutex_unlock (&lock);
        	handleJob(val);
        	val = '0';
        }
	}
	pthread_exit((void*)0);
}


void createThreads(void){
	int i,retcode;
	void* retVal[5];
	
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
			write(ERR,"ERROR In pthread_mutex_init\n",28);
			EXIT;
    }
    if (pthread_mutex_init(&writeLock, NULL) != 0)
    {
		write(ERR,"ERROR In pthread_mutex_init\n",28);
		EXIT;
    }
    if (pthread_mutex_init(&countLock, NULL) != 0)
    {
		write(ERR,"ERROR In pthread_mutex_init\n",28);
		EXIT;
    }
	for(i = 0; i < 5; i++) {
		retcode = pthread_create(&th[i], NULL, dealWithPick, NULL);
		if (retcode != 0) {
			write(ERR,"ERROR In pthread_create\n",24);
			EXIT;
		}	
	}


  for(i = 0; i < 5; i++)
    pthread_join(th[i], &retVal[i]);
}


void createSHM(void) {
	
	key = ftok("result.txt", 'H');
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
}


void createFile(void) {
	resultFD = open(FILE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (resultFD < 0) {
		write(ERR,"ERROR In open\n",13);
		EXIT;
	}
}

void initAll(void) {
	printf("File\n");
	createFile();
	printf("SHM\n");
	createSHM();
	printf("SEMA\n");
	createSema();
	printf("threads\n");
	createThreads();
	printf("queue\n");
	initQueue();
	printf("counter\n");
	initCounter();
}

void destroySHM(void) {
	shmdt(addr);
}

void cancelOther(void) {
	int i,retcode;
	for(i = 0; i < 5; i++) {
		pthread_cancel(th[i]);
  	}
  	writeToResults();
  	destroySHM();
  	kill(0,SIGCHLD);
  	createThreads();
  	createSHM();
}

void run(void) {
	char detail;
	while (TRUE) {
		printf("While 4ever\n");
		sopServer->sem_op = -1;
        semop ( semSerKey , sopClient , 1 );
        printf("IN server\n");
        detail = addr[0];
        if (detail == '7') {
        	cancelOther();
        }
        addNode(detail);
        sopServer->sem_op = 1;
		semop ( clieSemid , sopServer , 1 );
	}
}

int main(void) {
	initAll();
	run();
}