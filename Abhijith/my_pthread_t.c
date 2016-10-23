//#define DATA_TYPE pthread
#include <ucontext.h> 
#include <stdio.h>
#include <stdlib.h>
#include "my_pthread_t.h"
#include <signal.h>
#include <sys/time.h>


#define RUNTIME 25000

int num_threads = 0;
int runningthreads = 0;
int waitingid = 0;
int currentrunningid = -1;
pthread * currentthread = NULL;
int isThreadRunning = 0;

void init()
{
	my_pthread_yield();
}

void rq()
{
	nl[0] = my_thread_running_list;
	nl[1] = my_thread_running_list;
}

//This method schedules the next thread that is supposed to run. The interrupt must be raised every some particular time so that after a time slice a new thread gets a chance to run. 
void scheduler(int sig)
{
	printf("Inside the scheduler function \n");
	//if(runningthreads > 0)
	 my_pthread_yield();
	
	//Check if the running queue is not empty then select a new thread to run from that. For now we can do random order or FCFS but later implement multilevel feedback priority queue.
}

void prescheduler(void (*func)(void))
{
	printf("In Prescheduler function \n");
	struct sigaction interrupt_scheduler;
	struct itimerval it;
	
	interrupt_scheduler.sa_handler = scheduler;
	sigemptyset(&interrupt_scheduler.sa_mask);
	interrupt_scheduler.sa_flags = 0;
	
	sigaction(SIGALRM,&interrupt_scheduler,NULL);
	
	it.it_interval.tv_sec = 0;
 	it.it_interval.tv_usec = RUNTIME;
 	it.it_value.tv_sec = 0;
 	it.it_value.tv_usec = RUNTIME;
 	
 	setitimer(ITIMER_REAL, &it,NULL);
 	
 	func();
 	
 	//Since the function is done. Remove the thread from the running queue.
 	int c = 0;
 	for(c = currentrunningid; c< runningthreads -1; c++)
 		my_thread_running_list[c] = my_thread_running_list[c+1];
 		
 	//Reduce the number of running threads	
 	runningthreads--;
 	
 	printf("End of function in thread \n");
 	printf("The value of number of running threads in prescheduler is : %d \n",runningthreads);
 	if(runningthreads > 0)
	 	my_pthread_yield();
}

//The first argument is the buffer which stores the new thread information that is created successfully in this method. 
int my_pthread_create(pthread *thread,  void (*function)(void), void * arg)
{
	if(num_threads == MAX_THREADS)
		return 1;
	
	ucontext_t curr;
	getcontext(&curr);
	
	getcontext(&((*thread).mycontext));
	
	//Now have to makecontext to bind the context created above and the function passed as argument.
	(*thread).threadid = num_threads;
	(*thread).mycontext.uc_link=0;
	(*thread).stack=malloc(MEM);	
	(*thread).mycontext.uc_stack.ss_sp=malloc(MEM);
	(*thread).mycontext.uc_stack.ss_size = MEM;
	(*thread).mycontext.uc_stack.ss_flags = 0;
	(*thread).waitjoin = -1;	
	makecontext(&((*thread).mycontext), (void*)&prescheduler,1,(void*)function);
	
	//function();
	++num_threads;
	
	my_thread_list[num_threads] = thread;
	//Adding the newly created thread to the running queue.
	my_thread_running_list[runningthreads] = thread;
	runningthreads++;	
	
	//swapcontext(&curr,&((*thread).mycontext));
	
	printf("Thread created successfully\n");
	
	//prescheduler();
	 
	return 0;
}

//Scheduler will call this method after a certain time and we have to schedule another thread to run
//FCFS for now from the running threads
void my_pthread_yield()
{
	printf("Inside Yielding \n");

	if(isThreadRunning)
	{
		printf("Switching to main thread \n");
		swapcontext(&((*currentthread).mycontext),&main_context);		
	}
	else
	{
		currentrunningid = (currentrunningid + 1) % num_threads;		
		currentthread = my_thread_running_list[currentrunningid];
		printf("The threadid of the current running thread is : %d \n",(*currentthread).threadid);
		isThreadRunning = 1;
		swapcontext(&main_context,&((*currentthread).mycontext));
		isThreadRunning = 0;		
		printf("Return from swapcontext \n");
	}

	printf("Yielding is done \n");
}

void my_pthread_exit(void *value_ptr)
{
	//Exit the current thread.
	printf("thread %d is inside exiting\n", currentrunningid);
	if(currentthread->waitjoin != -1){
		//set the waitfor join thread runnable
		my_thread_list[currentthread->waitjoin]->status = value_ptr;
		//TODO set my_pthread_list[currentthread->waitjoin] to run
	}
	free(currentthread->mycontext.uc_stack.ss_sp);
	free(currentthread);	
	runningthreads--;
	num_threads--;
	my_thread_list[currentrunningid] = NULL;	
	isThreadRunning = 0;
	my_pthread_yield();
}
int my_pthread_join(pthread *thread,  void **value_ptr){
	printf("thread %d is inside join\n", currentrunningid);
	thread->waitjoin = currentthread->threadid;	
	value_ptr = &(currentthread->status);
	// TODO set currentthread waiting
	//
	isThreadRunning = 0;
	runningthreads--;
	my_pthread_yield();
}

void initialize()
{
	printf("Inside the initialize function \n");
	printf("The number of running threads is : %d \n",runningthreads);
	
	while(runningthreads > 0)	
		my_pthread_yield();
	
	printf("Initialization is done \n");
}
