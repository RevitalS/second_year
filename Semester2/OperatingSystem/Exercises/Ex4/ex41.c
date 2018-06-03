/******************
 * Shachar sirotkin
 * 208994962
 * ex4.c
 * 89231- 06
 ******************/
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

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

/**
* function name: main
* The output:0 on succses, else 1
* function operation: handle the client side
*/
int main() {
    int shmid;
    char *shm;
    key_t key = ftok("208994962.txt", 'k');
    CheckError(key, "ftok failed");
    CheckError((shmid = shmget(key, 1, 0666)),"shmget failed");
    //attach the segment to local data.
    if ((shm = (char *) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    struct sembuf sops[2];
    int semid;
    CheckError((semid = semget(key, 2, 0)), "shmget failed");

    //read semop
    sops[0].sem_flg = 0;
    sops[0].sem_op = 1;
    sops[0].sem_num = 0;
    //write semop
    sops[1].sem_flg = 0;
    sops[1].sem_op = -1;
    sops[1].sem_num = 1;

    char jobKey[2];
    while (1) {
        CheckError((int) write(1, "Please enter request code\n", 26),
                   "write failed");
        CheckError ((int)read(0, jobKey, 2),"read failed");
        jobKey[0] = (char) tolower(jobKey[0]);
        if (jobKey[0] == 'i') break;
        else {
            //lock writing to shared memory
            if (semop(semid, &sops[1], 1) == -1) {
                if (errno == 43 || errno == 22) {
                    CheckError((int) write(1, "semaphore was removed\n", 22),
                               "write Failed");
                    exit(0);
                }
                perror("semop failed");
                exit(1);
            }
            shm[0] = jobKey[0];
            //increment the read lock
            CheckError(semop(semid, &sops[0], 1), "semop failed");
        }
    }
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