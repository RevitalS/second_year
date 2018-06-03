//10_7
//thread synchronization problem - mutex solution
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[10];
int counter=0;

pthread_mutex_t lock;

void* doSomeThing(void *arg)
{
    unsigned long i = 0;
    pthread_mutex_lock(&lock);
    for(i=0; i<1000000;i++)
		counter++;
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(void)
{
    int i = 0;
    int err;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    while(i < 10)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
	//printf("created\n");
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }
	for (i=0;i<10;i++)
		pthread_join(tid[i], NULL);
    printf("counter value is: %d\n",counter);

    pthread_mutex_destroy(&lock);

    return 0;
}
