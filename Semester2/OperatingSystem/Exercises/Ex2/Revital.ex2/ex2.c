/******************
 * Shachar sirotkin
 * 208994962
 * ex2.c
 * 89231- 06
 ******************/

#include <sys/fcntl.h> // for open
#include <unistd.h> // for close
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

void Finish(int sig);
void CheckError(int x, char* s);

pid_t pid1, pid2;

/*******
 * func name: main
 * input: no input
 * output: 0
 * func operation: exec ex2_inp and ex2_upd
 * */
int main(int argc, char* argv[]) {
    int y = atoi(argv[1]);
    struct sigaction alarm_action;
    sigset_t block_mask;
    // Establish the signal handler.
    sigfillset(&block_mask);
    alarm_action.sa_handler = Finish;
    alarm_action.sa_mask = block_mask;
    alarm_action.sa_flags = 0;
    sigaction(SIGALRM, &alarm_action, NULL);

    alarm((unsigned int)y);
    int fd[2];
    CheckError(pipe(fd), "pipe error\n");
    CheckError(pid1 = fork(), "Failed forking\n");
    //This is the son.
    if (pid1 == 0) {
        char str[10];
        sprintf(str, "%d", fd[0]);
        execlp("./ex2_inp.out", "./ex2_inp.out", str, NULL);
        CheckError(-1, "failed exec ex2_inp.out\n");
    } else {
        CheckError(pid2 = fork(), "Failed forking\n");
        //This is the son.
        if (pid2 == 0) {
            char str1[10], str2[10];
            sprintf(str1, "%d", pid1);
            sprintf(str2, "%d", fd[1]);
            execlp("./ex2_upd.out", "./ex2_upd.out", str1, str2, NULL);
            CheckError(-1, "failed exec ex2_upd.out\n");
            //This is the father
        } else {
            while (1);
        }
    }
    return 0;
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

/*******
 * func name: Finish
 * input: the signal
 * output: no output
 * func operation: end the process
 */
void Finish(int sig) {
    kill(pid2, SIGINT);
    kill(pid1, SIGINT);
    exit(0);
}