/******************
 * Revital Shifman
 * 203438973
 * ex2.c
 * 89-231- 01
 ******************/
#include <sys/fcntl.h> // for open
#include <unistd.h> // for close
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BOARDSIZE 4
#define PRINTSIZE 5*BOARDSIZE*BOARDSIZE -1

int board[BOARDSIZE][BOARDSIZE];
int waitTime;
pid_t Pid;
struct sigaction alarm_action;

void init();
void run();
void add_new_num();
void printBoardAndSig();
void moveUp();
void moveDown();
void moveLeft();
void moveRight();
void click(char c);
void newWaitTime();
void checkError(int x, char* s);
int fd;
int main(int argc, char* argv[]) {
	Pid = atoi(argv[1]);
	fd = atoi(argv[2]);
	init(board);
	//Rand time for wait.
	waitTime = rand() % 5 + 1;
	run();
	//Send sigusr.
	return 0;
}

/***************
 * Init
 ************/
void init() {
	int i, j;
	//init board.
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
	printBoardAndSig();
}

/*****************
 * run - the game
 *****************/
void run() {
	char c;
	sigset_t block_mask;
	/* Establish the signal handler. */
	sigfillset(&block_mask);
	alarm_action.sa_handler = add_new_num;
	alarm_action.sa_mask = block_mask;
	alarm_action.sa_flags = 0;
	sigaction(SIGALRM, &alarm_action, NULL);

	alarm(waitTime);
	while (1) {
		system("stty cbreak -echo");
		c = getchar();
		system("stty cooked echo");
		click(c);
	}
}

/***************************
 * add new number to board
 **************************/
void add_new_num() {
	int i, j, count = 0, x;
	int arr[2][15];
	alarm_action.sa_handler = add_new_num;
	sigaction(SIGALRM, &alarm_action, NULL);
	newWaitTime();
	alarm(waitTime);
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
	printBoardAndSig();
}

/**********************************
 * print the board and send signal
 *********************************/
void printBoardAndSig() {
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
	strcat(m,"\n");
	int temp = dup(1);
	dup2(fd, 1);
	write(1, m, 5 * BOARDSIZE * BOARDSIZE - 1);
	dup2(1,temp);
	kill(Pid, SIGUSR1);
}
/***********************
 * MoveUp-
 * change the board up
 ***********************/
void moveUp() {
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

/***********************
 * MoveDown-
 * change the board down
 ***********************/
void moveDown() {
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

/***********************
 * MoveLeft-
 * change the board left
 ***********************/
void moveLeft() {
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

/***********************
 * MoveRight-
 * change the board right
 ***********************/
void moveRight() {
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

/***************************************
 * click- get char.
 * if had click move by the char we got.
 ****************************************/
void click(char c) {
	alarm(0);
	switch (c) {
	case 'A':
		moveLeft();
		printBoardAndSig();
		break;
	case 'D':
		moveRight();
		printBoardAndSig();
		break;
	case 'W':
		moveUp();
		printBoardAndSig();
		break;
	case 'X':
		moveDown();
		printBoardAndSig();
		break;
	case 'S':
		init();
		break;
	default:
		break;
	}
	alarm(waitTime);
}

/***************************
 * atoi - from char to int
 **************************/
int atoi(const char* c) {
	int nInt1 = 0;
	int i = 0;
	while (c[i]) {
		if (!nInt1)
			nInt1 = ((int) c[i]) - 48;
		else {
			int x = ((int) c[i] - 48);
			nInt1 = (nInt1 * 10) + x;
		}
		i++;
	}
	return (nInt1);
}

void newWaitTime() {
	waitTime = rand() % 5 + 1;
}

/************************************************
 * CheckError -
 * check if correct if not write error and exit
 ************************************************/
void checkError(int x, char* s) {
	if (x == -1) {
		int i = strlen(s);
		write(2, s, i);
		_exit(-1);
	}
}

