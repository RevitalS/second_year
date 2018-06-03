/******************
 * Shachar sirotkin
 * 208994962
 * ex2.c
 * 89231- 06
 ******************/
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#define BOARDSIZE 4

int board[BOARDSIZE][BOARDSIZE];
int waitTime;
pid_t Pid;
struct sigaction alarm_action;

void Initialize();

void CheckError(int x, char *s);

void Run();

void AddNumber(int sig);

void PrintBoard();

void MoveUp();

void MoveDown();

void MoveLeft();

void MoveRight();

void Click(char c);

void SetNewWaitTime();

void SIGINIT_Handler(int sig);

int fd;

/*******
 * func name: main
 * input: arguments to function. shall contain the pid of the inp process
 * output: always 0.
 * func operation: get the pid, initialize initial board and run the whole
 * changes handling on the board.
 */
int main(int argc, char *argv[]) {
    struct sigaction InitSigAc;
    sigset_t block_mask;
    //Establish the signal handler.
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGINT);
    InitSigAc.sa_handler = SIGINIT_Handler;
    InitSigAc.sa_mask = block_mask;
    InitSigAc.sa_flags = 0;
    CheckError(sigaction(SIGINT, &InitSigAc, NULL),
               "SIGINT sigaction failed");

    Pid = atoi(argv[1]);
    fd = atoi(argv[2]);
    Initialize();
    //Rand time for wait.
    waitTime = rand() % 5 + 1;
    Run();
    //Send sigusr.
    return 0;
}

/*******
 * func name: Initialize
 * input: no input
 * output: no output
 * func operation: initialize initial board
 */
void Initialize() {
    int i, j;
    //Initialize board.
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            board[i][j] = 0;
        }
    }
    i = rand() % 4;
    j = rand() % 4;
    board[i][j] = 2;
    i = rand() % 4;
    j = rand() % 4;
    board[i][j] = 2;
    PrintBoard();
}

/*******
 * func name: AddNumber
 * input: the alarm signal
 * output: no output
 * func operation: add new number two empty cell on the board.
 */
void AddNumber(int sig) {
    int i, j, count = 0, x;
    int arr[2][15];
    alarm_action.sa_handler = AddNumber;
    CheckError(sigaction(SIGALRM, &alarm_action, NULL),
               "SIGALRM sigaction failed");
    SetNewWaitTime();
    alarm((unsigned int) waitTime);
    //check if there is empty places.
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            if (board[i][j] == 0) {
                arr[0][count] = i;
                arr[1][count] = j;
                count++;
            }
        }
    }
    //insert new num to board.
    if (count) {
        x = rand() % (count - 1);
        board[arr[0][x]][arr[1][x]] = 2;
    } else {
        //Game Over.
    }
    PrintBoard();
}

/*******
 * func name: Run
 * input: no input
 * output: no output
 * func operation: run the whole changes handling on the board.
 */
void Run() {
    char c;
    sigset_t block_mask;
    // Establish the signal handler
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGINT);
    alarm_action.sa_handler = AddNumber;
    alarm_action.sa_mask = block_mask;
    alarm_action.sa_flags = 0;
    CheckError(sigaction(SIGALRM, &alarm_action, NULL),
               "SIGALRM sigaction failed");

    alarm(waitTime);
    while (1) {
        system("stty cbreak -echo");
        c = getchar();
        system("stty cooked echo");
        Click(c);
    }
}

/*******
 * func name: SIGINIT_Handler
 * input: the SIGINIT signal
 * output: no output
 * func operation: exit the game when eccept siginit signal
 */
void SIGINIT_Handler(int sig) {
    exit(0);
}

/*******
 * func name: MoveUp
 * input: no input
 * output: no output
 * func operation: move up every number on the board.
 */
void MoveUp() {
    int i, j, count = 0;
    //Move everything up
    for (j = 0; j < BOARDSIZE; j++) {
        for (i = 0; i < BOARDSIZE; i++) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i - count][j] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
    //binding to same num.
    for (i = 0; i < BOARDSIZE - 1; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i + 1][j]) {
                board[i][j] = board[i][j] * 2;
                board[i + 1][j] = 0;
            }
        }
    }

    //Move everything up
    for (j = 0; j < BOARDSIZE; j++) {
        for (i = 0; i < BOARDSIZE; i++) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i - count][j] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
}

/*******
 * func name: MoveDown
 * input: no input
 * output: no output
 * func operation: move down every number on the board.
 */
void MoveDown() {
    int i, j, count = 0;
    //Move everything down
    for (j = BOARDSIZE - 1; j >= 0; j--) {
        for (i = BOARDSIZE - 1; i >= 0; i--) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i + count][j] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
    //binding to same num.
    for (j = BOARDSIZE - 1; j >= 0; j--) {
        for (i = BOARDSIZE - 1; i > 0; i--) {
            if (board[i][j] != 0 && board[i][j] == board[i - 1][j]) {
                board[i][j] = board[i][j] * 2;
                board[i - 1][j] = 0;
            }
        }
    }
    //Move everything down
    for (j = BOARDSIZE - 1; j >= 0; j--) {
        for (i = BOARDSIZE - 1; i >= 0; i--) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i + count][j] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
}

/*******
 * func name: MoveLeft
 * input: no input
 * output: no output
 * func operation: move left every number on the board.
 */
void MoveLeft() {
    int i, j, count = 0;
    //Move everything left
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i][j - count] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
    //binding to same num.
    for (j = 0; j < BOARDSIZE - 1; j++) {
        for (i = 0; i < BOARDSIZE; i++) {
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1]) {
                board[i][j] = board[i][j] * 2;
                board[i][j + 1] = 0;
            }
        }
    }

    //Move everything left
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i][j - count] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
}

/*******
 * func name: MoveRight
 * input: no input
 * output: no output
 * func operation: move right every number on the board.
 */
void MoveRight() {
    int i, j, count = 0;
    //Move everything right
    for (i = BOARDSIZE - 1; i >= 0; i--) {
        for (j = BOARDSIZE - 1; j >= 0; j--) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i][j + count] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
    //binding to same num.
    for (i = BOARDSIZE - 1; i >= 0; i--) {
        for (j = BOARDSIZE - 1; j > 0; j--) {
            if (board[i][j] != 0 && board[i][j] == board[i][j - 1]) {
                board[i][j] = board[i][j] * 2;
                board[i][j - 1] = 0;
            }
        }
    }
    //Move everything right
    for (i = BOARDSIZE - 1; i >= 0; i--) {
        for (j = BOARDSIZE - 1; j >= 0; j--) {
            if (board[i][j] == 0) {
                count++;
            } else {
                if (count > 0) {
                    board[i][j + count] = board[i][j];
                    board[i][j] = 0;
                }
            }
        }
        count = 0;
    }
}

/*******
 * func name: Click
 * input: c - char represents the last accepted char
 * output: no output
 * func operation: updates board's movement according to c.
 *                 write the board state to stdout and signal
 *                 the printing process to print the matrix.
 */
void Click(char c) {
    alarm(0);
    switch (c) {
        case 'A':
            MoveLeft();
            PrintBoard();
            break;
        case 'D':
            MoveRight();
            PrintBoard();
            break;
        case 'W':
            MoveUp();
            PrintBoard();
            break;
        case 'X':
            MoveDown();
            PrintBoard();
            break;
        case 'S':
            Initialize();
            break;
        default:
            break;
    }
    alarm((unsigned int) waitTime);
}

/*******
 * func name: printBoard
 * input: no input
 * output: no output
 * func operation: print the board.
 */
void PrintBoard() {
    int i, j;
    char m[5 * BOARDSIZE * BOARDSIZE - 1];
    char one[6];
    memset(m, '\0', 5 * BOARDSIZE * BOARDSIZE - 1);
    //Print the board.
    for (i = 0; i < BOARDSIZE; i++) {
        for (j = 0; j < BOARDSIZE; j++) {
            memset(one, '\0', 6);
            if (i == 0 && j == 0) {
                sprintf(one, "%d", board[i][j]);
                strcat(m, one);
            } else {
                sprintf(one, ",%d", board[i][j]);
                strcat(m, one);
            }
        }
    }
    strcat(m, "\n");
    int temp = dup(1);
    CheckError(temp, "dup failed");
    CheckError(dup2(fd, 1), "dup2 failed");
    CheckError((int) write(1, m, 5 * BOARDSIZE * BOARDSIZE - 1),
               "write failed");
    CheckError(dup2(1, temp), "dup2 failed");
    CheckError(kill(Pid, SIGUSR1), "send SIGUSR1 to the process failed");
}

/*******
 * func name: SetNewWaitTime
 * input: no input
 * output: no output
 * func operation: set new wait time.
 */

void SetNewWaitTime() {
    waitTime = rand() % 5 + 1;
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