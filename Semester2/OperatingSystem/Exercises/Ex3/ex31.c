/******************
 * Shachar Sirotkin
 * 208994962
 * ex31.c
 * 89-231- 06
 ******************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#include <sys/fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */
#define FIFO_NAME "fifo_clientTOserver"
#define SIDE_SIZE 8
#define START_PLACE1 1
#define START_PLACE2 2
#define BOOLEAN int
#define FALSE 0
#define TRUE 1
#define NOTHING 2

/*******
 * func name: CheckError
 * input: x - a number represents aresult of system call.
 *        s - a string for perror if the system call failed
 * output: no output
 * func operation: checking whether the x num meant
 *                 that the sysytem call failed.
 *                 write perror and finish the program if it failed.
 */
void CheckError(int x, char* s);
/*******
 * func name: EndGame
 * func operation: wait till one of the clients send end game message
 *                 and pronounce the winner
 */
void EndGame();

char* data;
/*******
 * func name: main
 * output: shall be 0, if syscall failed -1
 * func operation: create fifo for getting both clients pid's
 *                 create shared memory for the game communication.
 */
int main() {
	key_t key;
	int shmid;
	int fifo;
    char pid1Str[10];
    char pid2Str[10];
	int pid1, pid2, check;
	//Create shared memory.
	CheckError(key = ftok("ex31.c", 'k'), "ftok\n");
	/* connect to (and possibly create) the segment: */
	CheckError(shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT), "shmget\n");
	/* attach to the segment to get a pointer to it: */
	data = (char*) shmat(shmid, NULL, 0);
	CheckError((data == NULL), "shmat\n");
	//Create FIFO.
	fifo = mkfifo(FIFO_NAME, 0666);
	CheckError(fifo, "make fifo");
	CheckError(fifo = open(FIFO_NAME, O_RDONLY), "open fifo\n");
	//Read from fifo for pid1.
	while (1) {
		CheckError(check = read(fifo, &pid1Str,10), "read fifo\n");
        pid1 = atoi(pid1Str);
		if (check > 0) {
			break;
		}
	}
	//Read from fifo for pid2.
	while (1) {
		CheckError(check = read(fifo, &pid2Str,10), "read fifo\n");
        pid2 = atoi(pid2Str);
		if (check > 0) {
			break;
		}
	}
	//close fifo.
	CheckError(unlink(FIFO_NAME),"unlinking fifo failed");
    CheckError(close(fifo),"closing fifo failed");
	data[0] = 0;
    CheckError(kill(pid1, SIGUSR1),"sending signal to first client failed");
	//Wait for the first move.
	while (data[0] == 0);
    CheckError(kill(pid2, SIGUSR1),"sending signal to second client failed");
    //wait till game ends and pronounce the winner
	EndGame();
    CheckError(shmctl(shmid, IPC_RMID, NULL), "shmctl\n");
    return 0;
}

/*******
 * func name: EndGame
 * func operation: wait till one of the clients send end game message
 *                 and pronounce the winner
 */
void EndGame() {
	char temp[4];
	strcpy(temp,data);
    int i=0;
	while (*data != 'g'){
		if(*data == 'b' || *data == 'w'){
			data+=4;
            i++;
        }
		sleep(1);
	}
	data+=2;
	char winner = *data;
    CheckError((int)write(1, "GAME OVER\n", 10),"writing GAME OVER failed");
	if (winner == 'b') {
        CheckError((int)write(1, "Winning player: Black\n", 22),
                   "writing Winning player: Black failed");
	} else if (winner == 'w') {
        CheckError((int)write(1, "Winning player: White\n", 22),
                   "writing Winning player: White failed");
	} else {
        CheckError((int)write(1, "No winning player\n", 18),
                   "writing No winning player failed");
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
void CheckError(int x, char* s) {
	if (x == -1) {
		perror (s);
		exit(-1);
	}
}
