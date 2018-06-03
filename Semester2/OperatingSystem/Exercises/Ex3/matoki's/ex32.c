


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER_SIZE 90
#define FIFO "fifo_clientTOserver"
#define SHM_SIZE 1024



/*
 * Waits till new writing to shared memory;
 */
void waitForWritingToTheAddres(char* shmaddr);
/*
 *Initialize the game table
 */
void initTable(char reversiTable[10][10]);
/*
 * Recursively,if necessary, flip the cell in given direction
 */
int moveToDirection(char table[10][10], int y, int dy, int x, int dx);
/*
 * Prints the table.
 */
void printTable(char reversiTable[10][10]);
/*
 * Puts coin in the [x,y].
 * if illegal returns 0
 */
int moveTo(char table[10][10], int y, int x);
/*
 * Parses the move from user input.
 */
int parseMove(char * coordinate, int *x, int *y);
/**
 * Does the opponent move
 */
void oponnentMove(char reversiTable[10][10], char* sharedAdd);
/*
 * Activates loop that reads/writes moves from shared memory,
 * according the data update state of the game
 */
void startGame(char reversiTable[10][10], char * sharedAdd);
/*
 * Set the signal again.
 */
void func(int sig);
/*
 * Takes move from shared memory.
 */
int parseMoveFromSharedMem(char * coordinate, int *x, int *y);
/*
 * Checks who is the winner when the boarder if full.
 */
void declareWinner(char reversiTable[10][10]);
/*
 * Creates shared memory with spatial key.
 */
void createSharedMem(int* shmid, key_t* key, char **shm);
/*
 * Reads one line from stdIn
 */
void readLine(char* buffer);
/*
 * Check if there is any available move to the player.
 */
int canMove(char reversiTable[10][10]);
/*
 * Declare winner when player stuck.
 */
void finishBecauseStuck();

char myColor;
char oposite;

int main() {
	signal(SIGUSR1, (void (*)(int)) func);
	char reversiTable[10][10];
	char buffer[11];
	srand(time(NULL));

	int fdWrite = open(FIFO, O_WRONLY);
	if (fdWrite < 0){//the file didn't open properly
		write(2, "Error open connection file file\n", 31);
		exit(1);
	}
	//Writes my pid
	snprintf(buffer, 10, "%d", getpid());
	if(0>write(fdWrite, buffer, strlen(buffer))){
		write(2, "Error writing to file\n", 25);
		close(fdWrite);
		exit(1);
	}


	int shmid;
	key_t key;
	char *shm;

	createSharedMem(&shmid, &key, &shm);

	//wait for SIGUSR1 to continue
	pause();

	//Make my own table
	initTable(reversiTable);

	//start send and receive moves
	startGame(reversiTable, shm);

	close(fdWrite);
	return 0;
}
/*
 * Creates shared memory with spatial key.
 */
void createSharedMem(int* shmid, key_t* key, char **shm){
	// make the key:
	if ((*key = ftok("ex31.c", 'k')) == -1) {
		write(2, "ftok error\n", 31);
		exit(1);
	}

	// Locate the segment.
	if ((*shmid = shmget(*key, SHM_SIZE, 0666)) < 0) {
		write(2, "shmget error\n", 31);
		exit(1);
	}


	// Now we attach the segment to our data space.
	if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
		write(2, "shmat error\n", 31);
		exit(1);
	}
}
/*
 * Set the signal again.
 */
void func(int sig) {
	signal(SIGUSR1, (void (*)(int)) func);
}
/*
 * Parses the move from user input.
 */
int parseMove(char * coordinate, int *x, int *y) {
	if (coordinate[0] == '[' && coordinate[4] == ']')
		if (coordinate[2] == ',')
			if (isdigit(coordinate[1]) && isdigit(coordinate[3])) {
				*x = (coordinate[1] - '0');
				*y = (coordinate[3] - '0');
				if ((*x) <= 0 || (*y) <= 0 || (*x) > 8 || (*y) > 8)
					return 0;
				return 1;
			}

	return 0;
}
/*
 * Takes move from shared memory.
 */
int parseMoveFromSharedMem(char * coordinate, int *x, int *y) {
	if (isdigit(coordinate[1]) && isdigit(coordinate[2])) {
		*x = (coordinate[1] - '0');
		*y = (coordinate[2] - '0');
		if ((*x) <= 0 || (*y) <= 0 || (*x) > 8 || (*y) > 8)
			return 0;
		return 1;
	}

	return 0;
}

/*
 *Initialize the game table
 */
void initTable(char reversiTable[10][10]) {
	int i, j;
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			reversiTable[i][j] = '0';
		}
	}
	reversiTable[4][4] = 'W';
	reversiTable[5][5] = 'W';
	reversiTable[4][5] = 'B';
	reversiTable[5][4] = 'B';
	//pedding the boarders for not get out of range

	for (i = 0; i < 10; i++) {
		reversiTable[i][0] = 'X';
		reversiTable[0][i] = 'X';
		reversiTable[9][i] = 'X';
		reversiTable[i][9] = 'X';
	}
}
/*
 * Puts coin in the [x,y].
 * if illegal returns 0
 */
int moveTo(char table[10][10], int y, int x) {
	int canMove = 0;
	if (table[y][x] != '0')
		return 0;
	//right
	if (table[y][x + 1] == oposite)
		if (moveToDirection(table, y, 0, x + 1, 1))
			canMove = 1;
	//left
	if (table[y][x - 1] == oposite)
		if (moveToDirection(table, y, 0, x - 1, -1))
			canMove = 1;
	//up
	if (table[y - 1][x] == oposite)
		if (moveToDirection(table, y - 1, -1, x, 0))
			canMove = 1;
	//down
	if (table[y + 1][x] == oposite)
		if (moveToDirection(table, y + 1, 1, x, 0))
			canMove = 1;
	//up- right
	if (table[y - 1][x + 1] == oposite)
		if (moveToDirection(table, y - 1, -1, x + 1, 1))
			canMove = 1;
	//up-left
	if (table[y - 1][x - 1] == oposite)
		if (moveToDirection(table, y - 1, -1, x - 1, -1))
			canMove = 1;
	//down- right
	if (table[y + 1][x + 1] == oposite)
		if (moveToDirection(table, y + 1, 1, x + 1, 1))
			canMove = 1;
	//down- left
	if (table[y + 1][x - 1] == oposite)
		if (moveToDirection(table, y + 1, 1, x - 1, -1))
			canMove = 1;
	if (canMove)
		table[y][x] = myColor;
	return canMove;
}
/*
 * Recursively,if necessary, flip the cell in given direction
 */
int moveToDirection(char table[10][10], int y, int dy, int x, int dx) {
	int canMove = 0;
	if (table[y + dy][x + dx] == myColor) {
		table[y][x] = myColor;
		return 1;
	}

	if (table[y + dy][x + dx] == oposite)
		canMove = moveToDirection(table, y + dy, dy, x + dx, dx);
	if (canMove)
		table[y][x] = myColor;
	return canMove;
}
/*
 * Waits till new writing to shared memory;
 */
void waitForWritingToTheAddres(char* shmaddr) {
	while (strlen(shmaddr) == 0) {
		usleep(500);
	}
}
/*
 * Prints the table.
 */
void printTable(char reversiTable[10][10]) {
	int i, j;
	for (j = 0; j < 25; j++) {
		write(1, "_", 1);
	}
	write(1, "\n", 1);
	for (i = 1; i < 9; i++) {
		write(1, "|", 1);
		for (j = 1; j < 9; j++) {
			if(reversiTable[i][j]=='0'){
				write(1, "  |", 3);
				continue;
			}

			write(1, &reversiTable[i][j], 1);
			write(1, " |", 2);
		}
		write(1, "\n", 1);
		for (j = 0; j < 25; j++) {
			write(1, "_", 1);
		}
		write(1, "\n", 1);
	}
}
/**
 * Does the opponent move
 */
void oponnentMove(char reversiTable[10][10], char* sharedAdd) {
	int x = 0;
	int y = 0;
	parseMoveFromSharedMem(sharedAdd, &x, &y);
	char temp = myColor;
	myColor = oposite;
	oposite = temp;
	moveTo(reversiTable, y, x);
	temp = myColor;
	myColor = oposite;
	oposite = temp;
}
/*
 * Activates loop that reads/writes moves from shared memory,
 * according the data update state of the game
 */
void startGame(char reversiTable[10][10], char * sharedAdd) {
	char buffer[BUFFER_SIZE];
	int x = 0, y = 0;
	int ocupidCells;
	char myColLow;


	if (strlen(sharedAdd) == 0) //first play for this program
	{
		myColor = 'B';
		oposite = 'W';
	} else {//first play for opponent program
		myColor = 'W';
		oposite = 'B';
		oponnentMove(reversiTable, sharedAdd);
		sharedAdd += 4;
		printTable(reversiTable);
		write(1, "\n\n", 2);
	}
	myColLow = myColor + 32;


	if (myColor == 'B')
		ocupidCells = 4;
	else
		ocupidCells = 5;

	while (1) {
		if (!canMove(reversiTable)){
			finishBecauseStuck();
			break;
		}

		write(1, "Please choose a square\n", strlen("Please choose a square\n"));
		readLine(buffer);
		if (parseMove(buffer, &x, &y) == 0) {
			write(1, "No such square\n", strlen("No such square\n"));
			continue;
		}

		if (moveTo(reversiTable, y, x) == 0) {
			write(1, "This square is invalide\n", strlen("This square is invalide\n"));
			continue;
		}

		write(1, "The board is:\n", strlen("The board is:\n"));
		printTable(reversiTable);
		write(1, "\n\n", 2);

		write(1, "Waiting for the other player to make a move\n", strlen("Waiting for the other player to make a move\n"));

		//write the move to shared memory
		buffer[0] = myColLow;
		buffer[1] = x + '0';
		buffer[2] = y + '0';
		buffer[3] = '\0';
		strcpy(sharedAdd, buffer);
		sharedAdd += 4;

		ocupidCells++;
		if (ocupidCells == 64) {
			declareWinner(reversiTable);
			break;
		}

		//wait for opponent move
		while (strlen(sharedAdd) == 0) {
			sleep(1);
		}

		oponnentMove(reversiTable, sharedAdd);
		ocupidCells++;
		sharedAdd += 4;

		printTable(reversiTable);
		write(1, "\n\n", 2);

		if (ocupidCells == 64) {
			declareWinner(reversiTable);
			break;
		}
	}
}
/*
 * Checks who is the winner when the boarder if full.
 */
void declareWinner(char reversiTable[10][10]) {
int i,j,blackCounter=0;

	for (i = 1; i < 9; i++) {
		for (j = 1; j < 9; j++) {
			if(reversiTable[i][j]=='B')
				blackCounter++;
		}
	}
	if(blackCounter>32)
		write(1, "Winning player: Black",strlen("Winning player: Black"));
	else
		if(blackCounter<32)
			write(1, "Winning player: White",strlen("Winning player: White"));
		else
			write(1, "No winning player",strlen("No winning player"));
}
/*
 * Reads one line from stdIn
 */
void readLine(char* buffer) {
	char c;
	do {
		if (read(0, &c, 1) < 0) {/* means problem in reading */
			write(2, "Error reading file\n", 25);
			exit(1);
		}
		*(buffer++) = c;
	} while (c != '\n');
	*(buffer - 1) = '\0';
}
/*
 * Check if there is any available move to the player.
 */
int canMove(char reversiTable[10][10]){
	char copyReversiTable[10][10];
	int i,j;

	for (i=0;i<10;i++){
		for (j=0;j<10;j++){
			copyReversiTable[i][j]=reversiTable[i][j];
		}
	}
	for (i=1;i<9;i++){
		for (j=1;j<9;j++){
			if (moveTo(copyReversiTable,i,j))
				return 1;
		}
	}
	return 0;
}
/*
 * Declare winner when player stuck.
 */
void finishBecauseStuck(){
	if(myColor=='B')
		write(1, "Winning player: White",strlen("Winning player: White"));
	else
		write(1, "Winning player: Black",strlen("Winning player: Black"));
}
