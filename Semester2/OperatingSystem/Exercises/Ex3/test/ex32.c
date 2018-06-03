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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */
#define FIFO_NAME "fifo_clientTOserver"
#define BOARD_SIZE 16
#define SIDE_SIZE 4
#define START_PLACE1 1
#define START_PLACE2 2
#define BOOL int
#define FALSE 0
#define TRUE 1
#define NOTHING 2
void CheckError(int x, char* s);
void (*oldHandler)();
void Play(char* data);
BOOL OneMove();
BOOL CheckLegal(int row, int col );
BOOL CheckSpecificPlace(char x, int countO);
BOOL CheckUp(int row, int col);
BOOL CheckDown(int row, int col);
BOOL CheckRight(int row, int col);
BOOL CheckLeft(int row, int col);
BOOL CheckUpRight(int row, int col);
BOOL CheckUpLeft(int row, int col);
BOOL CheckDownRight(int row, int col);
BOOL CheckDownLeft(int row, int col);
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
BOOL ChecKIfThereIsMove();
void StartBoard();
void PrintBoard();
void WhoWon();
BOOL OppMove();
void Connect();

char board[SIDE_SIZE][SIDE_SIZE];
char myColor, opponentColor;
char* dataNow;

int main() {
	myColor = 'B';
	opponentColor = 'W';
	int fifo;
	int pid1, pid2, check;
	int fd;
	char strPid1[11];
	signal(SIGUSR1, (void(*)(int))Connect);

	//Create FIFO.
	//fifo = mkfifo(FIFO_NAME,0666);
	//CheckError(fifo, "make fifo\n");
	CheckError(fd = open(FIFO_NAME, O_WRONLY), "open fifo\n");
	pid1 = getpid();
	sprintf(strPid1, "%d", pid1);
	write(fd, &pid1, strlen(strPid1));
	pause();
	Play(dataNow);
	return 0;
}

/**
 * connect to the shared memory.
 */
void Connect() {
	key_t key;
	int shmid;
	char *data;
	//Create shared memory.
	CheckError(key = ftok("ex31.c", 'k'), "ftok\n");
	/* connect to (and possibly create) the segment: */
	CheckError(shmid = shmget(key, SHM_SIZE, 0), "shmget\n");
	/* attach to the segment to get a pointer to it: */
	data = (char*) shmat(shmid, NULL, 0);
	CheckError((data == NULL), "shmat\n");
	dataNow = data;
	return;
}

/**
 * Play - the logic of the game.
 */
void Play(char* data) {
	StartBoard();
	//The game.
	if (strlen(data) == 0) {
		myColor = 'B';
		opponentColor = 'W';
	} else {
		myColor = 'W';
		opponentColor = 'B';
		write(1, "Waiting for the other player to make a move\n", 44);
		while (*data != 'b');
		myColor = 'B';
		opponentColor = 'W';
		AddToBoard(data[2] - '0', data[1] - '0');
		ChangeBoard(data[2] - '0', data[1] - '0');
		myColor = 'W';
		opponentColor = 'B';
		dataNow = (dataNow + 3);
	}
	PrintBoard();
	while (TRUE) {
		//dataNow = data;
		if (!OneMove()) {
			return;
		}
		PrintBoard();
		if (!OppMove()) {
			return;
		}
		PrintBoard();
	}

}

/**
 * Printing the board.
 */
void PrintBoard() {
	const int stam = 100;
	int i, j;
	char pb[stam];
	write(1, "-----------------\n", 18);
	for (i = 0; i < SIDE_SIZE; i++) {
		for (j = 0; j < SIDE_SIZE; j++) {
			if (j == SIDE_SIZE - 1) {
				pb[0] = '|';
				pb[1] = board[i][j];
				pb[2] = '|';
				pb[3] = '\n';
				write(1, pb, 4);
			} else {
				pb[0] = '|';
				pb[1] = board[i][j];
				write(1, pb, 2);
			}
		}
		write(1, "-----------------\n", 18);
	}
}

/**
 * One move of the player.
 */
BOOL OneMove() {
	char buff[25];
	char toTheShareMemo[4];
	int x, y;
	char temp;
	//doing one move.
	while (TRUE) {
		write(1, "Please choose a square\n", 23);

		CheckError(read(0, buff, 25), "can't read");
		if (buff[0] == '[' && buff[2] == ',' && buff[4] == ']') {
			x = buff[1] - 48;
			y = buff[3] - 48;
			if (x >= 1 && x <= 8 && y >= 1 && y <= 8) {
				if (board[y - 1][x - 1] == ' ' && CheckLegal(y - 1, x - 1)) {
					AddToBoard(y - 1, x - 1);
					ChangeBoard(y - 1, x - 1);
					toTheShareMemo[0] = myColor + 32;
					toTheShareMemo[1] = buff[1] - 1;
					toTheShareMemo[2] = buff[3] - 1;
					toTheShareMemo[3] = 0;
					strcpy(dataNow, toTheShareMemo);
					dataNow = (dataNow + 3);
					//Check if there is no move to opponent if not so the game is over.
						temp = myColor;
						myColor = opponentColor;
						opponentColor = temp;
						if (!ChecKIfThereIsMove()) {
							WhoWon();
							return FALSE;
						}
						temp = myColor;
						myColor = opponentColor;
						opponentColor = temp;
						return TRUE;
						break;
				} else {
					write(1, "This square is invalid\n", 23);
				}
			} else {
				write(1, "No such square\n", 15);
			}
		} else {
			write(1, "No such square\n", 15);
		}
	}
	return TRUE;
}

/**
 * Opp move.
 */
BOOL OppMove() {
	char temp;
	write(1, "Waiting for the other player to make a move\n", 44);
	//while the opponent not write.
	while (*dataNow != opponentColor + 32) {
		sleep(1);
	}
	temp = myColor;
	myColor = opponentColor;
	opponentColor = temp;
	AddToBoard(dataNow[2] - '0', dataNow[1] - '0');
	ChangeBoard(dataNow[2] - '0', dataNow[1] - '0');
	temp = myColor;
	myColor = opponentColor;
	opponentColor = temp;
	dataNow = (dataNow + 3);
	//Check if I have nove if not print game over.
	if (!ChecKIfThereIsMove()) {
		WhoWon();
		return FALSE;
	}
	return TRUE;
}

/**
 * Check if there is legal move in the board.
 */
BOOL ChecKIfThereIsMove() {
	int i, j;
	for (i = 0; i < SIDE_SIZE; i++) {
		for (j = 0; j < SIDE_SIZE; j++) {
			if (board[i][j] == ' ' && CheckLegal(i, j))
				return TRUE;
		}
	}
	return FALSE;
}

/**
 * the init board.
 */
void StartBoard() {
	int i, j;
	for (i = 0; i < SIDE_SIZE; i++) {
		for (j = 0; j < SIDE_SIZE; j++) {
			board[i][j] = ' ';
		}
	}
	board[START_PLACE1][START_PLACE1] = 'W';
	board[START_PLACE2][START_PLACE2] = 'W';
	board[START_PLACE1][START_PLACE2] = 'B';
	board[START_PLACE2][START_PLACE1] = 'B';
}

/**
 * Adding mew step to board.
 */
void AddToBoard(int row, int col) {
	board[row][col] = myColor;
}

/**
 * Change all what we need in the board.
 */
void ChangeBoard(int row, int col) {
	if (CheckUp(row, col))
		ChangeUp(row, col);
	if (CheckDown(row, col))
		ChangeDown(row, col);
	if (CheckLeft(row, col))
		ChangeLeft(row, col);
	if (CheckRight(row, col))
		ChangeRight(row, col);
	if (CheckUpRight(row, col))
		ChangeUpRight(row, col);
	if (CheckUpLeft(row, col))
		ChangeUpLeft(row, col);
	if (CheckDownRight(row, col))
		ChangeDownRight(row, col);
	if (CheckDownLeft(row, col))
		ChangeDownLeft(row, col);
}

/**
 * change up.
 */
void ChangeUp(int row, int col) {
	int i;
	i = row - 1;
	while (board[i][col] == opponentColor) {
		board[i][col] = myColor;
		i--;
	}
}

/**
 * change down.
 */
void ChangeDown(int row, int col) {
	int i;
	i = row + 1;
	while (board[i][col] == opponentColor) {
		board[i][col] = myColor;
		i++;
	}
}

/**
 * change left.
 */
void ChangeLeft(int row, int col) {
	int i;
	i = col - 1;
	while (board[row][i] == opponentColor) {
		board[row][i] = myColor;
		i--;
	}
}

/**
 * change right.
 */
void ChangeRight(int row, int col) {
	int i;
	i = col + 1;
	while (board[row][i] == opponentColor) {
		board[row][i] = myColor;
		i++;
	}
}

/**
 * change up right.
 */
void ChangeUpRight(int row, int col) {
	int i, j;
	i = row - 1;
	j = col + 1;
	while (board[i][j] == opponentColor) {
		board[i][j] = myColor;
		i--;
		j++;
	}
}

/**
 * change right down.
 */
void ChangeDownRight(int row, int col) {
	int i, j;
	i = row + 1;
	j = col + 1;
	while (board[i][j] == opponentColor) {
		board[i][j] = myColor;
		i++;
		j++;
	}
}

/**
 * change up left.
 */
void ChangeUpLeft(int row, int col) {
	int i, j;
	i = row - 1;
	j = col - 1;
	while (board[i][j] == opponentColor) {
		board[i][j] = myColor;
		i--;
		j--;
	}
}

/**
 * change down left.
 */
void ChangeDownLeft(int row, int col) {
	int i, j;
	i = row + 1;
	j = col - 1;
	while (board[i][j] == opponentColor) {
		board[i][j] = myColor;
		i++;
		j--;
	}
}

/***
 * Check if you stay in this place.
 */
BOOL CheckLegal(int row, int col) {
	if (CheckUp(row, col))
		return TRUE;
	else if (CheckDown(row, col))
		return TRUE;
	else if (CheckLeft(row, col))
		return TRUE;
	else if (CheckRight(row, col))
		return TRUE;
	else if (CheckUpRight(row, col))
		return TRUE;
	else if (CheckUpLeft(row, col))
		return TRUE;
	else if (CheckDownRight(row, col))
		return TRUE;
	else if (CheckDownLeft(row, col))
		return TRUE;
	return FALSE;
}

/**
 * Check if there is step up.
 */
BOOL CheckUp(int row, int col) {
	int i, countO = 0;
	BOOL bool;
	for (i = row - 1; i >= 0; i--) {
		bool = CheckSpecificPlace(board[i][col], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;

}

/**
 * Check if there is step down.
 */
BOOL CheckDown(int row, int col) {
	int i, countO = 0;
	BOOL bool;
	for (i = row + 1; i < SIDE_SIZE; i++) {
		bool = CheckSpecificPlace(board[i][col], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if there is step left.
 */
BOOL CheckLeft(int row, int col) {
	int i, countO = 0;
	BOOL bool;
	for (i = col - 1; i >= 0; i--) {
		bool = CheckSpecificPlace(board[row][i], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;

}

/**
 * Check if there is step right.
 */
BOOL CheckRight(int row, int col) {
	int i, countO = 0;
	BOOL bool;
	for (i = col + 1; i < SIDE_SIZE; i++) {
		bool = CheckSpecificPlace(board[row][i], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if there is step up and right.
 */
BOOL CheckUpRight(int row, int col) {
	int i, j, countO = 0;
	BOOL bool;
	for (i = row - 1, j = col + 1; i >= 0 && j < SIDE_SIZE; i--, j++) {
		bool = CheckSpecificPlace(board[i][j], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if there is step up and left.
 */
BOOL CheckUpLeft(int row, int col) {
	int i, j, countO = 0;
	BOOL bool;
	for (i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
		bool = CheckSpecificPlace(board[i][j], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if there is step down and right.
 */
BOOL CheckDownRight(int row, int col) {
	int i, j, countO = 0;
	BOOL bool;
	for (i = row + 1, j = col + 1; i < SIDE_SIZE && j < SIDE_SIZE; i++, j++) {
		bool = CheckSpecificPlace(board[i][j], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if there is step down and left.
 */
BOOL CheckDownLeft(int row, int col) {
	int i, j, countO = 0;
	BOOL bool;
	for (i = row + 1, j = col - 1; i < SIDE_SIZE && j >= 0; i++, j--) {
		bool = CheckSpecificPlace(board[i][j], countO);
		if (bool == NOTHING) {
			countO++;
		} else {
			return bool;
		}
	}
	return FALSE;
}

/**
 * Check if specific place. switch case.
 */
BOOL CheckSpecificPlace(char x, int countO) {
	if (x == ' ') {
		return FALSE;
	} else if (x == myColor) {
		if (countO == 0) {
			return FALSE;
		} else {
			return TRUE;
		}
	} else if (x == opponentColor) {
		return NOTHING;
	}
	return FALSE;
}

/**
 * Count to check who won.
 */
void WhoWon() {
	int countW = 0, countB = 0;
	int i, j;
	char temp;
	for (i = 0; i < SIDE_SIZE; i++) {
		for (j = 0; j < SIDE_SIZE; j++) {
			temp = board[i][j];
			if (temp == 'B') {
				countB++;
			} else if (temp == 'W') {
				countW++;
			}
		}
	}
	write(1, "GAME OVER\n", 10);
	if (countB > countW) {
		write(1, "Winning player: Black\n", 22);
	} else if (countW > countB) {
		write(1, "Winning player: White\n", 22);
	} else {
		write(1, "No winning player\n", 18);
	}
}

/************************************************
 * CheckError -
 * check if correct if not write error and exit
 ************************************************/
void CheckError(int x, char* s) {
	if (x == -1) {
		int i = strlen(s);
		write(2, s, i);
		_exit(-1);
	}
}

