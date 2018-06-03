/******************
 * Shachar sirotkin
 * 208994962
 * ex2.c
 * 89231- 06
 ******************/
#include <sys/fcntl.h> // for open
#include <unistd.h> // for close
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAT_SIZE 80
#define BOARDSIZE 4

int fd;

void CheckError(int x, char *s);

void PrintMatrix(int sig);

void Finish(int sig);

struct sigaction user_action;

/*******
 * func name: main
 * input: args.
 * output: 0
 * func operation: set signal actions.
 */

int main(int argc, char *argv[]) {
    fd = atoi(argv[1]);
    sigset_t block_mask;
    /* Establish the signal handler. */
    sigfillset(&block_mask);
    user_action.sa_handler = PrintMatrix;
    sigdelset(&block_mask, SIGINT);
    user_action.sa_mask = block_mask;
    user_action.sa_flags = 0;
    sigaction(SIGUSR1, &user_action, NULL);
    user_action.sa_handler = Finish;
    sigaction(SIGINT, &user_action, NULL);
    while (1);
}

/*******
 * func name: Finish
 * input: the signal
 * output: no output
 * func operation: end the process
 */
void Finish(int sig) {
    CheckError((int)write(1, "BYE BYE\n", 8),"write failed");
    exit(1);
}

/*******
 * func name: PrintMatrix
 * input: the signal
 * output: no output
 * func operation: print the board matrix
 */
void PrintMatrix(int sig) {
    user_action.sa_handler = PrintMatrix;
    sigaction(SIGUSR1, &user_action, NULL);
    int i, j, a = 0, x = 0, count = 0;
    char c[4];
    char pMat[84] = {'0'};
    char mat[MAT_SIZE];
    int temp = dup(0);
    dup2(fd, 0);
    CheckError((int) read(0, mat, MAT_SIZE), "can't read matrix in print");
    dup2(0, temp);
    while (mat[x] != '\n') {
        for (i = 0; i < BOARDSIZE; i++) {
            c[i] = '0';
        }
        i = 0;
        count = 0;
        while (mat[x] != ',' && mat[x] != '\n') {
            c[i] = mat[x];
            count++;
            i++;
            x++;
        }
        char d[4];
        if (count == 1 && c[0] == '0') {
            d[0] = d[1] = d[2] = d[3] = ' ';
            pMat[a++] = '|';
            for (j = 0; j < BOARDSIZE; j++) {
                pMat[a++] = d[j];
            }
            if (mat[x] == '\n') {
                break;
            }
            x++;
            continue;
        }
        for (i = 0; i < BOARDSIZE; i++) {
            d[i] = '0';
        }
        for (i = 0; i < count; i++) {
            d[BOARDSIZE - count + i] = c[i];
        }
        pMat[a++] = '|';
        for (j = 0; j < BOARDSIZE; j++) {
            pMat[a++] = d[j];
        }
        if (mat[x] == '\n') {
            break;
        }
        x++;
    }
    pMat[a] = '|';
    write(1, pMat, 21);
    write(1, "\n|", 2);
    write(1, pMat + 21, 20);
    write(1, "\n|", 2);
    write(1, pMat + 41, 20);
    write(1, "\n", 1);
    write(1, pMat + 60, 21);
    write(1, "\n", 1);
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
    if (x == -1) {
        size_t i = (size_t) strlen(s);
        write(2, s, i);
        exit(-1);
    }
}