#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 80
#define ROW_SIZE 30
#define ROWS_NUM 4
#define DELIM ","

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

void PrintMatrix(int sig);

void
CreateMatrix(char gameMatrix[ROWS_NUM][ROW_SIZE], char buffer[BUFFER_SIZE]);

void InsertNumberToMatrix(char *number, int position, int innerPosition,
                          char gameMatrix[ROWS_NUM][ROW_SIZE]);

int main() {
    struct sigaction SIGUSR1_Sigaction;
    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGINT);
    SIGUSR1_Sigaction.sa_handler = PrintMatrix;
    SIGUSR1_Sigaction.sa_mask = block_mask;
    SIGUSR1_Sigaction.sa_flags = 0;
    sigaction(SIGUSR1, &SIGUSR1_Sigaction, NULL);
    return 0;
}

void PrintMatrix(int sig) {
    char buffer[BUFFER_SIZE];
    char gameMatrix[ROWS_NUM][ROW_SIZE];
    CheckError((int) read(0, buffer, BUFFER_SIZE), "read from STDIN failed");
    CreateMatrix(gameMatrix, buffer);
}

void CreateMatrix(char gameMatrix[ROWS_NUM][ROW_SIZE],
                  char numbersState[BUFFER_SIZE]) {
    int i;
    for (i = 0; i < 4; i++) {
        strcpy(gameMatrix[i], "|      |      |      |      |");
    }
    char *currentNumber;
    currentNumber = strtok(numbersState, DELIM);
    int rowPosition = 0;
    int innerPosition = 0;
    while (currentNumber) {
        InsertNumberToMatrix(currentNumber, rowPosition, innerPosition,
                             gameMatrix);
        innerPosition++;
        if (innerPosition > 3) {
            innerPosition = 0;
            rowPosition++;
        }
        currentNumber = strtok(NULL, DELIM);
    }
}

void InsertNumberToMatrix(char *number, int position, int innerPosition,
                          char gameMatrix[ROWS_NUM][ROW_SIZE]) {
    int exactInnerPosition = 7 * innerPosition + 2;
    int i;
    switch (strlen(number)) {
        case 1:
            for (i = 0; i < 3; i++) {

            }
            break;
            exactInnerPosition += 3;
        case 2:
            exactInnerPosition += 2;
            break;
        case 3:
            exactInnerPosition += 1;
            break;
        default:
            break;
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
        size_t i = strlen(s);
        write(2, s, i);
        exit(-1);
    }
}