#include <stdio.h>
#include "my_pthread_t.h"
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#define INTERVAL 50000


void mainscheduler(int sig)
{
	printf("Inside the main scheduler function \n");
	//signal(SIGALRM,mainscheduler);
	//printf("%d milliseconds up, Wakeup!!!\n",50000/1000);
	//my_pthread_yield();
}

void test()
{
	printf("Hello I'm in test function \n");
}

void square()
{
	int i =0;
	for(i =0; i<= 500; i++)
	{
		//if(i == 250)
			//pause();
		printf("The current number is %d and the square of the number is %d \n",i,i*i);
	}
}

void squaresum()
{
	int sum = 0;
	int i = 0;
	for(i=0; i<= 50; i++)
	{
		sum += i*i;
		printf("The current number is %d and the sum of squares so far is %d \n",i, sum);
	}
}

void cube()
{
	int i =0;
	for(i =0; i<= 50; i++)
	{
		printf("The current number is %d and the cube of the number is %d \n",i,i*i*i);
	}
}

void cubesum()
{
	int sum = 0;
	int i =0;
	for(i = 0; i<= 50; i++)
	{
		sum += i*i*i;
		printf("The current number is %d and the sum of squares so far is %d \n",i, sum);
	}
}

void add10()
{
	int sum = 0;
	int i =0;
	for(i =0; i <= 50; i++)
	{
		sum += 10;
		printf("The current number is %d and the add10 is %d \n",i, sum);
	}
}

void add20()
{
	int sum = 0;
	int i =0;
	for(i =0; i <= 50; i++)
	{
		sum += 20;
		printf("The current number is %d and the add10 is %d \n",i, sum);
	}
}

int main(int argc, char *argv[])
{
	
	getcontext(&main_context);
	struct sigaction interrupt_mainscheduler;
	struct itimerval it;
	
	pthread temp;
	pthread squarethread,cubethread;
	printf("This is main function \n");
	
	
	//Do the testing here.
	//my_pthread_create(&temp, &test,0);
	my_pthread_create(&squarethread, &square,0);
	my_pthread_create(&cubethread, &cube,0);
	
	interrupt_mainscheduler.sa_handler = mainscheduler;
	interrupt_mainscheduler.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&interrupt_mainscheduler.sa_mask);
	//interrupt_mainscheduler.sa_flags = 0;
	//signal(SIGALRM,mainscheduler);
	if(sigaction(SIGALRM,&interrupt_mainscheduler,NULL) == -1)
	{
		printf("\n[Main Function]\t\tAn error occurred while initializing the signal handler 								for swapping to the scheduler context...");
		//exit(-1);
	}
	
	it.it_interval.tv_sec = 0;
 	it.it_interval.tv_usec = INTERVAL;
 	it.it_value.tv_sec = 0;
 	it.it_value.tv_usec = INTERVAL;
	//setitimer(ITIMER_REAL, &it,NULL);

	rq();	
	init();

	initialize();
	
	printf("THe program execution is over \n");
	return 0;
}
