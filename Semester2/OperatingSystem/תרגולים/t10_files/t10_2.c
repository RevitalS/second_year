//10_2
#include <pthread.h>
#include <stdio.h>

int int_Val[5] = {0,0,0,0,0};
pthread_t th[5];

void *add_to_value(void *arg) { // arg = 0, 1, 2, 3 ,4
  int inData = *((int*)arg);
  int i;
  for(i = 0; i < 10000; i++){
    int_Val[i % 5] += inData; /* ? */
   // printf("\n");
  }

  return (NULL);
}
int main(void) {
  int i, retcode;

  /* Create the threads */
  for(i = 0; i < 5; i++) {
	retcode = pthread_create(&th[i], NULL, add_to_value, (void *)&i);
	if (retcode != 0)
		printf("Create thread failed with error %d\n", retcode);
  }
 
  /* Wait until all threads have finished */
  void* retVal[5];
  for(i = 0; i < 5; i++)
    pthread_join(th[i], &retVal[i]);
 
  /* Print the results */
  printf("Final values:\n");
  for(i = 0; i < 5; i++)
    printf("Integer value[%d] = \t%d\n", i, int_Val[i]);
  return 0;
}
