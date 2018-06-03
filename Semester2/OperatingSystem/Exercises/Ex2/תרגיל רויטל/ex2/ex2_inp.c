/******************
 * Revital Shifman
 * 203438973
 * ex2_inp.c
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

#define MAT_SIZE 80
#define BOARDSIZE 4

int fd;
void checkError(int x, char* s);
void printMatrix();
void Finish(int sig);

struct sigaction user_action;
int main(int argc, char* argv[]) {
	fd = atoi(argv[1]);
	sigset_t block_mask;
	/* Establish the signal handler. */
	sigfillset(&block_mask);
	user_action.sa_handler = printMatrix;
	user_action.sa_mask = block_mask;
	user_action.sa_flags = 0;
	sigaction(SIGUSR1, &user_action, NULL);
	user_action.sa_handler = Finish;
	sigaction(SIGINT, &user_action, NULL);
	while (1);
}

void Finish(int sig) {
	write(1,"BYE BYE\n",8);
	_exit(1);
}

void printMatrix() {
	user_action.sa_handler = printMatrix;
	sigaction(SIGUSR1, &user_action, NULL);
	int i, j,a = 0, x = 0, count = 0;
	char c[4];
	char pMat[84] = { '0' };
	char mat[MAT_SIZE];
	int temp = dup(0);
	dup2(fd, 0);
	checkError(i = read(0, mat, MAT_SIZE), "can't read matrix in print");
//	printf("%s",mat);
	dup2(0,temp);
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
		if (count == 1 && c[0] == '0'){
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
	write(1,"\n|",2);
	write(1, pMat + 21, 20);
	write(1,"\n|",2);
	write(1, pMat + 41, 20);
	write(1,"\n",1);
	write(1, pMat + 60, 21);
	write(1,"\n",1);
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
