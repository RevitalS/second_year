


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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define BUFFER_SIZE 90
#define FIFONAME "fifo_clientTOserver"
#define SHM_SIZE 4096
#define FLAGS IPC_CREAT | 0644

/*
 * Reads the pid from the fifo file
 */
int getPidFromFifo(int fdRead);
/*
 * Waits till someone writes to shared memory
 */
void waitForWritingToTheAddres(char* shmaddr);
/*
 * Creates shared memory by spacial key
 */
void createSharedMem(int* shmid, key_t* key, char **shm);
/*
 * Takes move from shared memory.
 */
int parseMoveFromSharedMem(char * coordinate, int *x, int *y,char* color);
/*
 *Initialize the game table
 */
void initTable(char reversiTable[10][10]);
/*
 * Recursively,if necessary, flip the cell in given direction
 */
int moveToDirection(char table[10][10], int y, int dy, int x, int dx);
/*
 * Puts coin in the [x,y].
 * if illegal returns 0
 */
int moveTo(char table[10][10], int y, int x);
/*
 * Checks who is the winner when the boarder if full.
 */
void declareWinner(char reversiTable[10][10]);
/*
 * Updates the server board by the shared memory data.
 */
void listenToGame(char *shm);
/*
 * Check if there is any available move to the player.
 */
int canMove(char reversiTable[10][10]);
/*
 * Declare winner when player stuck.
 */
void finishBecauseStuck();
/*
 * Swaps colors of players
 */
void swapColors();

char myColor;
char oposite;

int main() {
	int shmid;
	key_t key;
	char *shm;
	//init the colors
	myColor='B';
	oposite='W';

	//creates shared memory
	createSharedMem(&shmid, &key, &shm);

	mkfifo(FIFONAME, 0666);
	int fdRead = open(FIFONAME, O_RDONLY);
	if (fdRead < 0){//the file didn't open properly
		write(2, "Error open connection file file\n", 31);
		exit(1);
	}
	//wait for sign from players
	int fid1 = getPidFromFifo(fdRead);
	int fid2 = getPidFromFifo(fdRead);
	kill(fid1, SIGUSR1);

	//waits for first move
	waitForWritingToTheAddres(shm);

	kill(fid2, SIGUSR1);

	//wait till wining
	listenToGame(shm);

	//closing the files
	close(fdRead);
	unlink(FIFONAME);

	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	}

	exit(0);
}
/*
 * Updates the server board by the shared memory data.
 */
void listenToGame(char *shm){
	int x,y;
	int ocupiedCells=4;

	char reversiTable[10][10];
	initTable(reversiTable);
	while (1){

		waitForWritingToTheAddres(shm);
		parseMoveFromSharedMem(shm, &x, &y,&myColor);
		if(myColor=='B')
			oposite='W';
		else
			oposite='B';
		moveTo(reversiTable,y,x);
		shm+=4;
		ocupiedCells++;
		if(ocupiedCells==64){
			declareWinner(reversiTable);
			break;
		}
		swapColors();
		if (canMove(reversiTable)==0){
			finishBecauseStuck();
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
 * Takes move from shared memory.
 */
int parseMoveFromSharedMem(char * coordinate, int *x, int *y,char* color) {
	if (isdigit(coordinate[1]) && isdigit(coordinate[2])) {
		*color=(coordinate[0] - 32);
		*x = (coordinate[1] - '0');
		*y = (coordinate[2] - '0');
		if ((*x) <= 0 || (*y) <= 0 || (*x) > 8 || (*y) > 8)
			return 0;
		return 1;
	}
	return 0;
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
 * Creates shared memory by spacial key
 */
void createSharedMem(int* shmid, key_t* key, char **shm) {

	// make the key:
	if ((*key = ftok("ex31.c", 'k')) == -1) {
		perror("ftok");
		exit(1);
	}

	// Create the segment.
	if ((*shmid = shmget(*key, SHM_SIZE, FLAGS)) < 0) {
		perror("shmget");
		exit(1);
	}

	// Now we attach the segment to our data space.
	if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
		perror("shmat");
		exit(1);
	}
}
/*
 * Reads the pid from the fifo file
 */
int getPidFromFifo(int fdRead) {
	char buffer[BUFFER_SIZE];
	int readedChars1 = read(fdRead, buffer, sizeof(buffer));
	while (readedChars1 == 0) {
		readedChars1 = read(fdRead, buffer, sizeof(buffer));
		usleep(500);
	}
	buffer[readedChars1] = '\0';
	return atoi(buffer);
}
/*
 * Waits till someone writes to shared memory
 */
void waitForWritingToTheAddres(char* shmaddr) {
	while (strlen(shmaddr) == 0) {
		usleep(500);
	}
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
/*
 * Swaps colors of players
 */
void swapColors(){
	char temp=myColor;
	myColor=oposite;
	oposite=temp;
}
