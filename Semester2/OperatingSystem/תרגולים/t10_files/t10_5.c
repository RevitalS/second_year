//10_5
//To compile: gcc -l pthread t10_5.c
#include<stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include<stdlib.h>
pthread_t ntid[2];
pthread_t ltid;
void* thr_fn(void *arg)
{
    int i;
    for(i=0;i<3;i++)
    {
	printf("my tid is %u, my pid is: %d, the loop value is: %d\n",pthread_self(),getpid(),i);
	sleep(2);
    }
    
    return((void *)0);
}
void* thr_fn2(void *arg)
{
    execl("/bin/ls", "ls", (char*)NULL);
    return((void *)0);
}

int main(void)
{
    int     err;
    int i;
    for(i=0;i<2;i++)
    	err = pthread_create(&ntid[i], NULL, thr_fn, NULL);
    if (err != 0)
	perror("can't create thread\n");
    
    sleep(1);
    pthread_create(&ltid, NULL, thr_fn2, NULL);
    pthread_exit((void*)0);
    printf("I'm done\n");
}
