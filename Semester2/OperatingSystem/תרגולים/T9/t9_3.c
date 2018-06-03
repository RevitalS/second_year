#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include<stdio.h>
#include<malloc.h>
#define NUMOFSEM 3

union semun {	
	int val;	// value for SETVAL 
	struct semid_ds *buf;	//buffer for IPC_STAT, IPC_SET 
	unsigned short *array;	// array for GETALL, SETALL 
};

main()
{
        int semid,value,i;
        union semun semarg,semarg1;

        semid=semget ( IPC_PRIVATE , NUMOFSEM , 0600 );
        semarg.array=(unsigned short*)  malloc ( NUMOFSEM*sizeof ( unsigned short  ) );
        semarg1.array=( unsigned short* ) malloc ( NUMOFSEM*sizeof ( unsigned short  ) );
        for ( i=0 ; i<NUMOFSEM ; i++ )
                semarg.array[i]=1;
        semctl ( semid , 0 , SETALL , semarg );
        semctl ( semid , 0 , GETALL , semarg1 );
        for ( i=0 ; i<NUMOFSEM ; i++ )
                printf ( "semaphore %d: %hu\n" , i , semarg1.array[i]);
        semarg.val=0;
        semctl ( semid , 0 , SETVAL , semarg );
        value=semctl ( semid , 0 , GETVAL , semarg );
        printf ( "\nsemaphore 0: %d\n" , value );
}

