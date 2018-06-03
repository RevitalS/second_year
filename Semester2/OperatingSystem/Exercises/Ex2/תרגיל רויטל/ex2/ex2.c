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

void finish();
void checkError(int x, char* s);
int atoi(const char* c);

#define BOARDSIZE 4
#define PRINTSIZE 5*BOARDSIZE*BOARDSIZE -1
pid_t pid1, pid2;
int main(int argc, char* argv[]) {
	int y = atoi(argv[1]);
	struct sigaction alarm_action;
	sigset_t block_mask;
	/* Establish the signal handler. */
	sigfillset(&block_mask);
	alarm_action.sa_handler = finish;
	alarm_action.sa_mask = block_mask;
	alarm_action.sa_flags = 0;
	sigaction(SIGALRM, &alarm_action, NULL);

	alarm(y);
	int fd[2];
	checkError(pipe(fd), "pipe error\n");
	checkError(pid1 = fork(), "Faile make fork\n");
	//This is the son.
	if (pid1 == 0) {
		char str[10];
		sprintf(str, "%d", fd[0]);
			execlp("./ex2_inp.out", "./ex2_inp.out", str, NULL);
			checkError(-1, "exe faile in run\n");
	} else {
		checkError(pid2 = fork(), "Faile make fork\n");
		//This is the son.
		if (pid2 == 0) {
			char str1[10], str2[10];
			sprintf(str1, "%d", pid1);
			sprintf(str2, "%d", fd[1]);
			execlp("./ex2_upd.out", "./ex2_upd.out", str1, str2, NULL);
			checkError(-1, "exe faile in run\n");
			//This is the father
		} else {
			while (1);
		}
	}
	return 0;
}

void finish() {
	kill(pid1, SIGINT);
	kill(pid2, SIGINT);
	_exit(0);
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
