#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int ctrl_c_count=0;
void (* old_handler) (int);
void ctrl_c(int);

int main()
{
	old_handler=signal(SIGINT,ctrl_c);
	if(old_handler==SIG_ERR)
	{
		printf("error registering function\n");
		return 1;
	}
	
	//reading all chars untill \n and counting number of ctrl+C
	while(getchar()!='\n');
	
	printf("ctrl-c count = %d\n",ctrl_c_count);
	signal(SIGINT,old_handler);

	for(;;);
	return 0;
}

void ctrl_c(int signum)
{
	signal(SIGINT,ctrl_c);
	++ctrl_c_count;
}
