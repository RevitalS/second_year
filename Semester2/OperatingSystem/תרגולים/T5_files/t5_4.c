#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>

void boom(int signum)
{
	printf("Hold me close, Red. I-I-it's gettin' dark.\n Tell aunty em to let old yeller out. \nTell Tiny Tim I won't be coming home this Christmas. \nTell Scarlett I do give a damn. \nPardon me. \nThank you! \nYou love me. \nYou really love me!");
	exit(1);
}
int main()
{
	signal(SIGUSR1, boom);
	while(1);
}

