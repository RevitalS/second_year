//10_6
//thread synchronization problem
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[10];
int counter=0;

void* doSomeThing(void *arg)
{
    unsigned long i = 0;
    
    for(i=0; i<1000000;i++)
		counter++;

    return NULL;
}

int main(void)
{
    int i = 0;
    int err;

    while(i < 10)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    for (i=0;i<10;i++)
    	pthread_join(tid[i], NULL);
    
	

    printf("counter value is: %d\n",counter);

    return 0;
}
