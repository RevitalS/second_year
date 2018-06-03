/******************
 * Shachar sirotkin
 * 208994962
 * ex4.c
 * 89231- 06
 ******************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>

int internal_count;

//lock for queue and lock for text file & internal_count

pthread_mutex_t queueLocker, fileLocker;
int fd;

// queue node
typedef struct queueNode {
    char data;
    struct queueNode *next;
} queueNode;

// queue struct
typedef struct queue {
    queueNode *head;
    queueNode *tail;
    int size;
} queue;

// semctl args struct
union semnum {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
* function name: DeleteQueue
* The input: que -pointer to queue
* The Function Operation: free all queue's resources
*/
void DeleteQueue(queue *que);

/**
* function name: CreateQueue
* output: pointer to new queue
* func operation: create a new queue and initialize it
*/
queue *CreateQueue();

/**
* function name: HandleJob
* The input: pointer to queue struct 
* funcn operation:  handle a task according to the next char in the queue
*/
void *HandleJob(void *que);

/**
* func name: AddNewJob
* The input: que - queue struct pointer
 *           element - a job to push to queue
* function operation: add an element to the queue
*/
void AddNewJob(queue *que, char element);

/**
* function name: WriteToFile
* func Operation: write the thread id and internal count to the file
*/
void WriteToFile();

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s);

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckPthreadError(int x, char *s);

/**
* func name: main
* The output: 0 on succses, else -1
* func Operation: initialize all resources and handle clients
*/
int main() {
    //get random number
    time_t t;
    srand((unsigned) time(&t));
    CheckError((fd = open("208994962.txt", O_CREAT | O_TRUNC | O_RDWR, 0666)),
               "open failed");
    int shmid;
    char *shm;
    key_t key = ftok("208994962.txt", 'k');
    if (key == -1) {
        perror("ftok failed");
        close(fd);
        exit(-1);
    }
    if ((shmid = shmget(key, 1, IPC_CREAT | 0666)) < 0) {
        perror("shmget failed");
        close(fd);
        exit(-1);
    }
    //attach the segment to local data.
    if ((shm = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(-1);
    }
    shm[0] = 0;

    struct sembuf sops[2];
    int semid;
    //get semaphores for read and write on the shared memory
    if ((semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        close(fd);
        perror("semget failed");
        exit(-1);
    }
    //init semaphores
    unsigned short array[2];
    array[0] = 0;
    array[1] = 1;
    union semnum arg;
    arg.val = 0;
    arg.array = array;
    if (semctl(semid, 0, SETALL, arg) < 0) {
        perror("semctl");
        close(fd);
        //delete the shared memory
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
        }
        //delete the semaphores
        if (semctl(semid, 0, IPC_RMID, NULL) < 0) {
            perror("semctl");
        }
        if (semctl(semid, 1, IPC_RMID, NULL) < 0) {
            perror("semctl");
        }
        exit(-1);
    }
    //read semop
    sops[0].sem_flg = 0;
    sops[0].sem_op = -1;
    sops[0].sem_num = 0;
    //write semop
    sops[1].sem_flg = 0;
    sops[1].sem_op = 1;
    sops[1].sem_num = 1;

    if (pthread_mutex_init(&queueLocker, NULL) != 0 ||
        pthread_mutex_init(&fileLocker, NULL) != 0) {
        perror("mutex init");
        close(fd);
        //delete shared memory
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
        }
        //delete semaphores
        if (semctl(semid, 0, IPC_RMID, NULL) < 0) {
            perror("semctl");
        }
        if (semctl(semid, 1, IPC_RMID, NULL) < 0) {
            perror("semctl");
        }
        exit(-1);
    }

    //create queue and thread pool
    queue *que = CreateQueue();
    pthread_t threadPool[5];
    int i;
    for (i = 0; i < 5; i++) {
        if (pthread_create(&threadPool[i], NULL, HandleJob, que) != 0) {
            perror("pthread_create");
            close(fd);
            //delete shared memory
            if (shmctl(shmid, IPC_RMID, NULL) < 0) {
                perror("shmctl");
            }
            //delete semaphores
            if (semctl(semid, 0, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            if (semctl(semid, 1, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            exit(-1);
        }
    }

    char jobChar;
    int finish = 0;

    //start handling clients
    while (!finish) {
        if (semop(semid, &sops[0], 1) == -1) {
            perror("semop");
            close(fd);
            //delete shared memory
            if (shmctl(shmid, IPC_RMID, NULL) < 0) {
                perror("shmctl");
            }
            //delete semaphores
            if (semctl(semid, 0, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            if (semctl(semid, 1, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            exit(-1);
        }
        jobChar = shm[0];
        //increase the write lock
        if (semop(semid, &sops[1], 1) == -1) {
            perror("semop");
            close(fd);
            //delete shared memory
            if (shmctl(shmid, IPC_RMID, NULL) < 0) {
                perror("shmctl");
            }
            //delete semaphores
            if (semctl(semid, 0, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            if (semctl(semid, 1, IPC_RMID, NULL) < 0) {
                perror("semctl");
            }
            exit(-1);
        }
        //close all threads
        if (jobChar == 'g') {
            CheckPthreadError(pthread_mutex_lock(&queueLocker),
                              "lock on queueLocker failed");
            CheckPthreadError(pthread_mutex_lock(&fileLocker),
                              "lock on fileLocker failed");
            for (i = 0; i < 5; i++) {
                CheckPthreadError(pthread_cancel(threadPool[i]),
                                  "pthread_cancel failed");
            }
            CheckPthreadError(pthread_mutex_unlock(&queueLocker),
                              "unlock on queueLocker failed");
            CheckPthreadError(pthread_mutex_unlock(&fileLocker),
                              "unlock on fileLocker failed");
            finish = 1;
        }
        else if (jobChar == 'h') {
            for (i = 0; i < 5; i++) {
                AddNewJob(que, jobChar);
            }
            finish = 1;
            //push new task to queue
        } else {
            AddNewJob(que, jobChar);
        }
    }

    for (i = 0; i < 5; i++) {
        CheckPthreadError(pthread_join(threadPool[i], NULL),
                          "join thread failed");
    }
    WriteToFile();
    //free all resources
    CheckPthreadError(pthread_mutex_destroy(&queueLocker),
                      "pthread_mutex_destroy failed on queueLocker");
    CheckPthreadError(pthread_mutex_destroy(&fileLocker),
                      "pthread_mutex_destroy failed on fileLocker");
    DeleteQueue(que);
    CheckError(shmctl(shmid, IPC_RMID, NULL), "shmctl failed");
    close(fd);
    CheckError(semctl(semid, 0, IPC_RMID, NULL), "shmctl failed");
    CheckError(semctl(semid, 1, IPC_RMID, NULL), "shmctl failed");
    return 1;
}

/**
* function name: RandNanoSleep
* The output: 0 if time end without signal.else, -1
* The Function Operation:select random num x between 10-100,
 *                       and sleep for x nano sec
*/
int RandNanoSleep() {
    struct timespec nanosec;
    nanosec.tv_sec = 0;
    nanosec.tv_nsec = (rand() % 91) + 10;
    return nanosleep(&nanosec, NULL);
}

/**
* func name: AddNewJob
* The input: que - queue struct pointer
 *           element - a job to push to queue
* function operation: add an element to the queue
*/
void AddNewJob(queue *que, char element) {
    //create new queueNode
    queueNode *ptr = (queueNode *) malloc(sizeof(struct queueNode));
    ptr->data = element;
    ptr->next = NULL;
    CheckPthreadError(pthread_mutex_lock(&queueLocker),
                      "lock on queueLocker failed");
    // if queue is empty, set new queueNode to be both head and tail
    if (que->head == NULL) {
        que->head = ptr;
        que->tail = ptr;
    }
        // queue isn't empty, set it to be the tail
    else {
        que->tail->next = ptr;
        que->tail = ptr;
    }
    //increasing queue size
    struct sembuf sb;
    sb.sem_flg = 0;
    sb.sem_op = 1;
    sb.sem_num = 0;
    CheckError(semop(que->size, &sb, 1), "semop failed");
    //unlock the queue
    CheckPthreadError(pthread_mutex_unlock(&queueLocker),
                      "unlock on queueLocker failed");

}

/**
* function name: GetJob
* The input: que - queue struct pointer
* The output: first element in the queue
* The Function Operation: pop the first element in the queue
*/
char GetJob(queue *que) {
    //decreasing queue size
    struct sembuf sb;
    sb.sem_flg = 0;
    sb.sem_op = -1;
    sb.sem_num = 0;
    CheckError(semop(que->size, &sb, 1), "semop failed");
    //lock queue
    CheckPthreadError(pthread_mutex_lock(&queueLocker),
                      "lock on queueLocker failed");
    //get first element
    queueNode *temp = que->head;
    char data = temp->data;
    //set head to the second element
    que->head = que->head->next;
    free(temp);
    //unlock  queue
    CheckPthreadError(pthread_mutex_unlock(&queueLocker),
                      "unlock on queueLocker failed");
    return data;
}

/**
* function name: WriteToFile
* func Operation: write the thread id and internal count to the file
*/
void WriteToFile() {
    char buff[2048];
    memset(buff, 0, 2048);
    //lock file and internal count
    CheckPthreadError(pthread_mutex_lock(&fileLocker),
                      "lock on fileLocker failed");
    sprintf(buff, "thread identifier is %lu and internal count is %d\n",
            pthread_self(), internal_count);
    if (write(fd, buff, strlen(buff)) < strlen(buff)) {
        perror("write:");
        //unlock file and internal count
        CheckPthreadError(pthread_mutex_unlock(&fileLocker),
                          "unlock on fileLocker failed");
        pthread_exit(NULL);
    }
    //unlock file and internal count
    CheckPthreadError(pthread_mutex_unlock(&fileLocker),
                      "unlock on fileLocker failed");

}

/**
* function name: AddNumToCounter
* The input: counter - pointer to counter
 *           num - number to add
* The Function Operation:   add num to counter in thread-safe manner
*/
void AddNumToCounter(int *counter, int num) {
    //lock counter
    CheckPthreadError(pthread_mutex_lock(&fileLocker),
                      "lock on fileLocker failed");
    *counter += num;
    //unlock counter
    CheckPthreadError(pthread_mutex_unlock(&fileLocker),
                      "unlock on fileLocker failed");
}

/**
* function name: HandleJob
* The input: pointer to queue struct
* funcn operation:  handle a task according to the next char in the queue
*/
void *HandleJob(void *que) {
    CheckPthreadError(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL),
                      "pthread_setcanceltype failed");
    while (1) {
        char job = GetJob((queue *) que);
        switch (job) {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
                if (RandNanoSleep() < 0) {
                    perror("nanosleep:");
                    pthread_exit(NULL);
                }
                AddNumToCounter(&internal_count, job + 1 - 'a');
                break;
            case 'f':
                WriteToFile();
                break;
            case 'h':
                WriteToFile();
                pthread_exit(NULL);
            default:
                break;
        }
    }
}

/**
* function name: CreateQueue
* output: pointer to new queue
* func operation: create a new queue and initialize it
*/
queue *CreateQueue() {
    queue *que = (queue *) malloc(sizeof(struct queue));
    //create private semaphore to control the queue
    CheckError((que->size = semget(IPC_PRIVATE, 1, 0666)), "semget failed");
    //initialize the semaphore to 0
    union semnum arg;
    arg.val = 0;
    arg.array = NULL;
    CheckError(semctl(que->size, 0, SETVAL, arg), "semctl failed");
    que->head = NULL;
    que->tail = NULL;
    return que;
}

/**
* function name: DeleteQueue
* The input: pointer to queue
* The Function Operation: free all queue's resources
*/
void DeleteQueue(queue *que) {
    //get queue size and delete all elements
    int size;
    CheckError((size = semctl(que->size, 0, GETVAL, NULL)), "semctl failed");
    while (size > 0) {
        GetJob(que);
        size--;
    }
    //delete queue semaphore
    CheckError(semctl(que->size, 0, IPC_RMID, NULL), "semctl failed");
    //free queue struct
    free(que);
}

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char *s) {
    //if x is -1, writing s to stderr
    if (x == -1) {
        perror(s);
        exit(-1);
    }
}

/*******
 * func name: CheckPthreadError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckPthreadError(int x, char *s) {
    //if x is not 0, writing s to stderr
    if (x == 0) {
        perror(s);
        exit(-1);
    }
}
