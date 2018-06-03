/******************
 * Revital Shifman
 * 203438973
 * ex31.c
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
#define BOOLEAN int
#define FALSE 0
#define TRUE 1
#define NOTHING 2
int Atoi(const char* c);
void CheckError(int x, char* s);
void Play(char* data);
void (*oldHandler)();
BOOLEAN OneMove(int row, int col);
BOOLEAN CheckLegal(int row, int col);
BOOLEAN CheckSpecificPlace(char x, int countO);
BOOLEAN CheckUp(int row, int col);
BOOLEAN CheckDown(int row, int col);
BOOLEAN CheckRight(int row, int col);
BOOLEAN CheckLeft(int row, int col);
BOOLEAN CheckUpRight(int row, int col);
BOOLEAN CheckUpLeft(int row, int col);
BOOLEAN CheckDownRight(int row, int col);
BOOLEAN CheckDownLeft(int row, int col);
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
BOOLEAN ChecKIfThereIsMove();
void StartBoard();
void WhoWon();

typedef struct Game {
	char board[SIDE_SIZE][SIDE_SIZE];
	char myColor;
	char opponentColor;
} Game;

Game game;
char* dataNow;
int main() {
	key_t key;
	int shmid;
	char *data;
	int fifo;
	int pid1, pid2, check;
	//Create shared memory.
	CheckError(key = ftok("ex31.c", 'k'), "ftok");
	/* connect to (and possibly create) the segment: */
	CheckError(shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT), "shmget\n");
	/* attach to the segment to get a pointer to it: */
	data = (char*) shmat(shmid, NULL, 0);
	CheckError((data == NULL), "shmat\n");

	//Create FIFO.
	fifo = mkfifo(FIFO_NAME, 0666);
	CheckError(fifo, "make fifo\n");
	CheckError(fifo = open(FIFO_NAME, O_RDONLY), "open fifo\n");
	//Read from fifo for pid1.
	while (1) {
		CheckError(check = read(fifo, &pid1, sizeof(int)), "read fifo\n");
		if (check > 0) {
			break;
		}
	}

	//Read from fifo for pid2.
	while (1) {
		CheckError(check = read(fifo, &pid2, sizeof(int)), "read fifo\n");
		if (check > 0) {
			break;
		}
	}
	//close fifo.
	unlink(FIFO_NAME);
	close(fifo);
	data[0] = 0;
	kill(pid1, SIGUSR1);
	//Wait for the first move.
	while (data[0] == 0);
	kill(pid2, SIGUSR1);
	StartBoard();
	game.myColor = 'b';
	game.opponentColor = 'w';
	char temp = data[2] - '0';
	printf("%d",temp);
	write(1, &temp, 1);
	AddToBoard(data[2] - '0', data[1] - '0');
	ChangeBoard(data[2] - '0', data[1] - '0');
	//strcpy(dataNow, "what write");
	dataNow = data;
	Play(data);
	CheckError(shmctl(shmid, IPC_RMID, NULL), "shmctl\n");
	return 0;
}

/**
 * the game logic.
 */
void Play(char* data) {
	dataNow = (dataNow + 3);
	game.myColor = 'w';
	game.opponentColor = 'b';

	while (1) {
		if (*dataNow == game.myColor) {
			if (!OneMove(data[2] - '0', data[1] - '0')) {
				return;
			}
			dataNow = (dataNow + 3);
		}
	}

}

/**
 * One move of the player.
 */
BOOLEAN OneMove(int row, int col) {
	char temp;
	//doing one move.
	AddToBoard(col - 1, row - 1);
	ChangeBoard(col - 1, row - 1);
		temp = game.myColor;
		game.myColor = game.opponentColor;
		game.opponentColor = temp;
		if (!ChecKIfThereIsMove()) {
			WhoWon();
			return FALSE;
		}
	return TRUE;

}

/**
 * Check if there is legal move in the board.
 */
BOOLEAN ChecKIfThereIsMove() {
	int i, j;
	for (i = 0; i < SIDE_SIZE; i++) {
		for (j = 0; j < SIDE_SIZE; j++) {
			if (game.board[i][j] == ' ' && CheckLegal(i, j))
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
			game.board[i][j] = ' ';
		}
	}
	game.board[START_PLACE1][START_PLACE1] = 'w';
	game.board[START_PLACE2][START_PLACE2] = 'w';
	game.board[START_PLACE1][START_PLACE2] = 'b';
	game.board[START_PLACE2][START_PLACE1] = 'b';
}

/**
 * Adding mew step to board.
 */
void AddToBoard(int row, int col) {
	game.board[row][col] = game.myColor;
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
	while (game.board[i][col] == game.opponentColor) {
		game.board[i][col] = game.myColor;
		i--;
	}
}

/**
 * change down.
 */
void ChangeDown(int row, int col) {
	int i;
	i = row + 1;
	while (game.board[i][col] == game.opponentColor) {
		game.board[i][col] = game.myColor;
		i++;
	}
}

/**
 * change left.
 */
void ChangeLeft(int row, int col) {
	int i;
	i = col - 1;
	while (game.board[row][i] == game.opponentColor) {
		game.board[row][i] = game.myColor;
		i--;
	}
}

/**
 * change right.
 */
void ChangeRight(int row, int col) {
	int i;
	i = col + 1;
	while (game.board[row][i] == game.opponentColor) {
		game.board[row][i] = game.myColor;
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
	while (game.board[i][j] == game.opponentColor) {
		game.board[i][j] = game.myColor;
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
	while (game.board[i][j] == game.opponentColor) {
		game.board[i][j] = game.myColor;
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
	while (game.board[i][j] == game.opponentColor) {
		game.board[i][j] = game.myColor;
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
	while (game.board[i][j] == game.opponentColor) {
		game.board[i][j] = game.myColor;
		i++;
		j--;
	}
}

/***
 * Check if you stay in this place.
 */
BOOLEAN CheckLegal(int row, int col) {
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
BOOLEAN CheckUp(int row, int col) {
	int i, countO = 0;
	BOOLEAN bool;
	for (i = row - 1; i >= 0; i--) {
		bool = CheckSpecificPlace(game.board[i][col], countO);
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
BOOLEAN CheckDown(int row, int col) {
	int i, countO = 0;
	BOOLEAN bool;
	for (i = row + 1; i < SIDE_SIZE; i++) {
		bool = CheckSpecificPlace(game.board[i][col], countO);
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
BOOLEAN CheckLeft(int row, int col) {
	int i, countO = 0;
	BOOLEAN bool;
	for (i = col - 1; i >= 0; i--) {
		bool = CheckSpecificPlace(game.board[row][i], countO);
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
BOOLEAN CheckRight(int row, int col) {
	int i, countO = 0;
	BOOLEAN bool;
	for (i = col + 1; i < SIDE_SIZE; i++) {
		bool = CheckSpecificPlace(game.board[row][i], countO);
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
BOOLEAN CheckUpRight(int row, int col) {
	int i, j, countO = 0;
	BOOLEAN bool;
	for (i = row - 1, j = col + 1; i >= 0 && j < SIDE_SIZE; i--, j++) {
		bool = CheckSpecificPlace(game.board[i][j], countO);
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
BOOLEAN CheckUpLeft(int row, int col) {
	int i, j, countO = 0;
	BOOLEAN bool;
	for (i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
		bool = CheckSpecificPlace(game.board[i][j], countO);
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
BOOLEAN CheckDownRight(int row, int col) {
	int i, j, countO = 0;
	BOOLEAN bool;
	for (i = row + 1, j = col + 1; i < SIDE_SIZE && j < SIDE_SIZE; i++, j++) {
		bool = CheckSpecificPlace(game.board[i][j], countO);
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
BOOLEAN CheckDownLeft(int row, int col) {
	int i, j, countO = 0;
	BOOLEAN bool;
	for (i = row + 1, j = col - 1; i < SIDE_SIZE && j >= 0; i++, j--) {
		bool = CheckSpecificPlace(game.board[i][j], countO);
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
BOOLEAN CheckSpecificPlace(char x, int countO) {
	if (x == ' ') {
		return FALSE;
	} else if (x == game.myColor) {
		if (countO == 0) {
			return FALSE;
		} else {
			return TRUE;
		}
	} else if (x == game.opponentColor) {
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
			temp = game.board[i][j];
			write(1, &temp, 1);
			if (temp == 'b') {
				countB++;
			} else if (temp == 'w') {
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

/***************************
 * Atoi - from char to int
 **************************/
int Atoi(const char* c) {
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

/************************************************
 * CheckError -
 * check if correct if not write error and exit
 ************************************************/
void CheckError(int x, char* s) {
	if (x == -1) {
		int i = strlen(s);
		perror (s);
		_exit(-1);
	}
}
