/******************
 * Revital Shifman
 * 203438973
 * ex32.c
 * 89-231- 01
 ******************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <sys/fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/wait.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */
#define FIFO_NAME "fifo_clientTOserver"
#define SIDE_SIZE 8
#define START_PLACE1 3
#define START_PLACE2 4
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
void CheckError(int x, char *s);

void Play(char *data);

BOOLEAN MyMove();

BOOLEAN IsLegalMove(int row, int col);

BOOLEAN CheckSpecificPlace(char x, int opponentDisksCounter);

BOOLEAN CheckUpLegality(int row, int col);

BOOLEAN CheckDownLegality(int row, int col);

BOOLEAN CheckRightLegality(int row, int col);

BOOLEAN CheckLeftLegality(int row, int col);

BOOLEAN CheckUpRightLegality(int row, int col);

BOOLEAN CheckUpLeftLegality(int row, int col);

BOOLEAN CheckDownRightLegality(int row, int col);

BOOLEAN CheckDownLeftLegality(int row, int col);

void AddToBoard(int row, int col);

void ChangeBoard(int row, int col);

void ChangeDown(int row, int col);

void ChangeLeft(int row, int col);

void ChangeUp(int row, int col);

void ChangeRight(int row, int col);

void ChangeDownLeft(int row, int col);

void ChangeDownRight(int row, int col);

void ChangeUpLeft(int row, int col);

void ChangeUpRight(int row, int col);

BOOLEAN CheckIfThereIsMove();

void StartBoard();

void PrintBoard();

void WhoWon();

BOOLEAN OppMove();

void Connect();

char board[SIDE_SIZE][SIDE_SIZE];
char myColor = '1', opponentColor = '2';
char myColorSymbol = 'w', opponentColorSymbol = 'b';
char printingColor, checkingColor;
char *dataNow;

/*******
 * func name: main
 * output: shall be 0, if syscall failed -1
 * func operation: send pid to server fifo, wait for signal and start game
 */
int main() {
    int pid1;
    int fd;
    char strPid1[10];
    //set SIGUSR1 function to be Connect
    signal(SIGUSR1, (void (*)(int)) Connect);
    //send the pid to server's fifo
    CheckError(fd = open(FIFO_NAME, O_WRONLY), "open fifo");
    pid1 = getpid();
    memset(strPid1,0,10);
    sprintf(strPid1, "%d", pid1);
    CheckError((int) write(fd, strPid1, strlen(strPid1)),
               "writing client pid to fifo failed");
    //wait for the SIGUSR1 signal
    pause();
    //start the game
    Play(dataNow);
    return 0;
}

/*******
 * func name: Connect
 * func operation: connect to server's shared memory
 */
void Connect() {
    key_t key;
    int shmid;
    char *data;
    //Create shared memory key.
    CheckError(key = ftok("ex31.c", 'k'), "ftok\n");
    /* connect to (and possibly create) the shared memory */
    CheckError(shmid = shmget(key, SHM_SIZE, 0), "shmget\n");
    /* attach to the shared memory to get a pointer to it */
    data = (char *) shmat(shmid, NULL, 0);
    CheckError((data == NULL), "shmat\n");
    dataNow = data;
    return;
}

/*******
 * func name: Play
 * input: data - pointer to the shared memory
 * func operation: start and run the game using the shard memory
 *                 for clients and server communication
 */
void Play(char *data) {
    //set initial board
    StartBoard();
    //if the client is the first to join the game he will be the black player
    if (dataNow[0] == 0) {
        myColor = '2';
        opponentColor = '1';
        myColorSymbol = 'b';
        opponentColorSymbol = 'w';
    } else {
        //if he is the second client he will be the white player
        // and will wait for his turn
        CheckError(
                (int) write(1, "Waiting for the other player to make a move\n",
                            44),
                "failed writing the  waiting message");
        while (*data != opponentColorSymbol) {
            sleep(1);
        }
        //add to board the opponent new disk
        printingColor = opponentColor;
        checkingColor = myColor;
        AddToBoard(data[2] - '0', data[1] - '0');
        //change board according to the opponent new disc
        ChangeBoard(data[2] - '0', data[1] - '0');
        dataNow = (dataNow + 4);
    }
    //print the update board
    PrintBoard();
    //continue menage turns till the game ends
    while (TRUE) {
        if (!MyMove()) {
            return;
        }
        //print the update board
        PrintBoard();
        if (!OppMove()) {
            return;
        }
        //print the update board
        PrintBoard();
    }
}

/*******
 * func name: PrintBoard
 * func operation: Printing the update board.
 */
void PrintBoard() {
    int i, j;
    char pb[2];
    for (i = 0; i < SIDE_SIZE; i++) {
        for (j = 0; j < SIDE_SIZE; j++) {
            //if its the last char in the row, add \n to it
            if (j == SIDE_SIZE - 1) {
                pb[0] = board[i][j];
                pb[1] = '\n';
                CheckError((int) write(1, pb, 2),
                           "writing char with endline to board failed");
            } else {
                pb[0] = board[i][j];
                CheckError((int) write(1, pb, 1),
                           "writing charto board failed");
            }
        }
    }
}

/*******
 * func name: MyMove
 * output: TRUE if there is possible next move. Else FALSE
 * func operation: get player move, adding it to board.
 *                 prints failing message in case the move is not legal
 */
BOOLEAN MyMove() {
    char buff[25];
    char toTheShareMemo[4];
    int col, row;
    //doing one move.
    while (TRUE) {
        CheckError((int) write(1, "Please choose a square\n", 23),
                   "failed writing Please choose a square");
        CheckError((int) read(0, buff, 25), "can't read new move");
        if (buff[0] == '[' && buff[2] == ',' && buff[4] == ']') {
            col = buff[1] - 48;
            row = buff[3] - 48;
            if (col >= 0 && col < 8 && row >= 0 && row < 8) {
                checkingColor = opponentColor;
                printingColor = myColor;
                if (board[row][col] == '0' && IsLegalMove(row, col)) {
                    AddToBoard(row, col);
                    ChangeBoard(row, col);
                    toTheShareMemo[0] = myColorSymbol;
                    toTheShareMemo[1] = buff[1];
                    toTheShareMemo[2] = buff[3];
                    toTheShareMemo[3] = 0;
                    strcpy(dataNow, toTheShareMemo);
                    dataNow = (dataNow + 4);
                    /*Check if there is no move to opponent.
                    if not so the game is over.*/
                    printingColor ^= checkingColor ^= printingColor ^=
                            checkingColor;
                    if (!CheckIfThereIsMove()) {
                        WhoWon();
                        return FALSE;
                    }
                    return TRUE;
                } else {
                    CheckError((int) write(1, "This square is invalid\n", 23),
                               "failed writing This square is invalid");
                }
            } else {
                CheckError((int) write(1, "No such square\n", 15),
                           "failed writing No such square");
            }
        } else {
            CheckError((int) write(1, "No such square\n", 15),
                       "failed writing No such square");
        }
    }
    return TRUE;
}

/*******
 * func name: OppMove
 * output: TRUE if there is possible next move. Else FALSE
 * func operation: get opponent move, adding it to board.
 *                 prints failing message in case the move is not legal
 */
BOOLEAN OppMove() {
    CheckError(
            (int) write(1, "Waiting for the other player to make a move\n",
                        44),
            "failed writing the  waiting message");
    //while the opponent not write.
    while (*dataNow != opponentColorSymbol) {
        sleep(1);
    }
    //adding opponent move to the board
    printingColor = opponentColor;
    checkingColor = myColor;
    AddToBoard(dataNow[2] - '0', dataNow[1] - '0');
    ChangeBoard(dataNow[2] - '0', dataNow[1] - '0');
    dataNow = (dataNow + 4);
    //Check if I have move, if not print game over.
    printingColor ^= checkingColor ^= printingColor ^= checkingColor;
    if (!CheckIfThereIsMove()) {
        WhoWon();
        return FALSE;
    }
    return TRUE;
}

/*******
 * func name: CheckIfThereIsMove
 * output: TRUE if there is possible next move. Else FALSE
 * func operation: Check if there is possible next legal move in the board.
 */
BOOLEAN CheckIfThereIsMove() {
    int i, j;
    for (i = 0; i < SIDE_SIZE; i++) {
        for (j = 0; j < SIDE_SIZE; j++) {
            if (board[i][j] == '0' && IsLegalMove(i, j))
                return TRUE;
        }
    }
    return FALSE;
}

/*******
 * func name: StartBoard
 * func operation: prints the init board.
 */
void StartBoard() {
    int i, j;
    for (i = 0; i < SIDE_SIZE; i++) {
        for (j = 0; j < SIDE_SIZE; j++) {
            board[i][j] = '0';
        }
    }
    board[START_PLACE1][START_PLACE1] = '2';
    board[START_PLACE2][START_PLACE2] = '2';
    board[START_PLACE1][START_PLACE2] = '1';
    board[START_PLACE2][START_PLACE1] = '1';
    board[START_PLACE2][START_PLACE1] = '1';
}

/*******
 * func name: AddToBoard
 * func operation: Adding the new move to board.
 */
void AddToBoard(int row, int col) {
    board[row][col] = printingColor;
}

/*******
 * func name: ChangeBoard
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board each disc that influenced by the new move
 */
void ChangeBoard(int row, int col) {
    //try to change every disc in every diredtion on the board
    if (CheckUpLegality(row, col))
        ChangeUp(row, col);
    if (CheckDownLegality(row, col))
        ChangeDown(row, col);
    if (CheckLeftLegality(row, col))
        ChangeLeft(row, col);
    if (CheckRightLegality(row, col))
        ChangeRight(row, col);
    if (CheckUpRightLegality(row, col))
        ChangeUpRight(row, col);
    if (CheckUpLeftLegality(row, col))
        ChangeUpLeft(row, col);
    if (CheckDownRightLegality(row, col))
        ChangeDownRight(row, col);
    if (CheckDownLeftLegality(row, col))
        ChangeDownLeft(row, col);
}

/*******
 * func name: ChangeUp
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the upper discs regarding the new one
 */
void ChangeUp(int row, int col) {
    int i;
    i = row - 1;
    while (board[i][col] == checkingColor) {
        board[i][col] = printingColor;
        i--;
    }
}

/*******
 * func name: ChangeDown
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the lower discs regarding the new one
 */
void ChangeDown(int row, int col) {
    int i;
    i = row + 1;
    while (board[i][col] == checkingColor) {
        board[i][col] = printingColor;
        i++;
    }
}

/*******
 * func name: ChangeLeft
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the left discs regarding the new one
 */
void ChangeLeft(int row, int col) {
    int i;
    i = col - 1;
    while (board[row][i] == checkingColor) {
        board[row][i] = printingColor;
        i--;
    }
}

/*******
 * func name: ChangeRight
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the right discs regarding the new one
 */
void ChangeRight(int row, int col) {
    int i;
    i = col + 1;
    while (board[row][i] == checkingColor) {
        board[row][i] = printingColor;
        i++;
    }
}

/*******
 * func name: ChangeUpRight
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the up and right discs regarding the new one
 */
void ChangeUpRight(int row, int col) {
    int i, j;
    i = row - 1;
    j = col + 1;
    while (board[i][j] == checkingColor) {
        board[i][j] = printingColor;
        i--;
        j++;
    }
}

/*******
 * func name: ChangeDownRight
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the down and right discs regarding the new one
 */
void ChangeDownRight(int row, int col) {
    int i, j;
    i = row + 1;
    j = col + 1;
    while (board[i][j] == checkingColor) {
        board[i][j] = printingColor;
        i++;
        j++;
    }
}

/*******
 * func name: ChangeUpLeft
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the up and left discs regarding the new one
 */
void ChangeUpLeft(int row, int col) {
    int i, j;
    i = row - 1;
    j = col - 1;
    while (board[i][j] == checkingColor) {
        board[i][j] = printingColor;
        i--;
        j--;
    }
}

/*******
 * func name: ChangeDownLeft
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * func operation: Change on board all the down and left discs regarding the new one
 */
void ChangeDownLeft(int row, int col) {
    int i, j;
    i = row + 1;
    j = col - 1;
    while (board[i][j] == checkingColor) {
        board[i][j] = printingColor;
        i++;
        j--;
    }
}

/*******
 * func name: IsLegalMove
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality
 */
BOOLEAN IsLegalMove(int row, int col) {
    //check move's legality regarding every direction on board
    if (CheckUpLegality(row, col))
        return TRUE;
    else if (CheckDownLegality(row, col))
        return TRUE;
    else if (CheckLeftLegality(row, col))
        return TRUE;
    else if (CheckRightLegality(row, col))
        return TRUE;
    else if (CheckUpRightLegality(row, col))
        return TRUE;
    else if (CheckUpLeftLegality(row, col))
        return TRUE;
    else if (CheckDownRightLegality(row, col))
        return TRUE;
    else if (CheckDownLeftLegality(row, col))
        return TRUE;
    return FALSE;
}

/*******
 * func name: CheckUpLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to up direction
 */
BOOLEAN CheckUpLegality(int row, int col) {
    int i;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row - 1; i >= 0; i--) {
        specificLegality = CheckSpecificPlace(board[i][col],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter++;
        } else {
            return specificLegality;
        }
    }
    return FALSE;

}

/*******
 * func name: CheckDownLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to down direction
 */
BOOLEAN CheckDownLegality(int row, int col) {
    int i;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row + 1; i < SIDE_SIZE; i++) {
        specificLegality = CheckSpecificPlace(board[i][col],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/*******
 * func name: CheckLeftLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to left direction
 */
BOOLEAN CheckLeftLegality(int row, int col) {
    int i;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = col - 1; i >= 0; i--) {
        specificLegality = CheckSpecificPlace(board[row][i],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;

}

/*******
 * func name: CheckRightLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to right direction
 */
BOOLEAN CheckRightLegality(int row, int col) {
    int i;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = col + 1; i < SIDE_SIZE; i++) {
        specificLegality = CheckSpecificPlace(board[row][i],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/*******
 * func name: CheckUpRightLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to up and right direction
 */
BOOLEAN CheckUpRightLegality(int row, int col) {
    int i, j;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row - 1, j = col + 1; i >= 0 && j < SIDE_SIZE; i--, j++) {
        specificLegality = CheckSpecificPlace(board[i][j],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/*******
 * func name: CheckUpLeftLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to up and left direction
 */
BOOLEAN CheckUpLeftLegality(int row, int col) {
    int i, j;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        specificLegality = CheckSpecificPlace(board[i][j],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/*******
 * func name: CheckDownRightLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to down and right direction
 */
BOOLEAN CheckDownRightLegality(int row, int col) {
    int i, j;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row + 1, j = col + 1; i < SIDE_SIZE && j < SIDE_SIZE; i++, j++) {
        specificLegality = CheckSpecificPlace(board[i][j],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/*******
 * func name: CheckDownLeftLegality
 * input: row - the row of the new disc
 *        col - the col of the new disc
 * output: TRUE if the move is legal, else FALSE
 * func operation: check move's legality regarding to down and left direction
 */
BOOLEAN CheckDownLeftLegality(int row, int col) {
    int i, j;
    BOOLEAN specificLegality, opponentDisksCounter = FALSE;
    for (i = row + 1, j = col - 1; i < SIDE_SIZE && j >= 0; i++, j--) {
        specificLegality = CheckSpecificPlace(board[i][j],
                                              opponentDisksCounter);
        if (specificLegality == NOTHING) {
            opponentDisksCounter = TRUE;
        } else {
            return specificLegality;
        }
    }
    return FALSE;
}

/**
 * Check if specific place. switch case.
 */
BOOLEAN CheckSpecificPlace(char x, BOOLEAN opponentDisksCounter) {
    if (x == '0') {
        return FALSE;
    } else if (x == printingColor) {
        if (opponentDisksCounter == FALSE) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return NOTHING;
}

/**
 * Count to check who won.
 */
void WhoWon() {
    PrintBoard();
    int countW = 0, countB = 0;
    int i, j;
    char temp;
    for (i = 0; i < SIDE_SIZE; i++) {
        for (j = 0; j < SIDE_SIZE; j++) {
            temp = board[i][j];
            if (temp == '2') {
                countB++;
            } else if (temp == '1') {
                countW++;
            }
        }
    }
    char endMsg[4];
    endMsg[0] = 'g';
    endMsg[1] = 'o';
    if (countB > countW) {
        endMsg[2] = 'b';
        write(1, "Winning player: Black\n", 22);
    } else if (countW > countB) {
        endMsg[2] = 'w';
        write(1, "Winning player: White\n", 22);
    } else {
        endMsg[2] = 'd';
        write(1, "No winning player\n", 18);
    }
    endMsg[3] = '\0';
    strcpy(dataNow, endMsg);
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
        perror(s);
        exit(-1);
    }
}

