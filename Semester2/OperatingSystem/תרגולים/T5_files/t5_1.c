#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int doneflag = 0;

void setdoneflag(int signo) {
    printf("\nIn SignalHandler - setdoneflag the signal that I got was %d\n",signo);
    doneflag = 1;
}

int main (void) {

    signal(SIGINT, setdoneflag);

    while (!doneflag) {
        printf("press CTRL+C to kill the Loop\n");
        sleep(1);
    }

    printf("Program terminating ...\n");
    return 0;
}
