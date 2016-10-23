//#define DATA_TYPE pthread
#include <ucontext.h> 
#include <stdio.h>
#include <stdlib.h>
#include "my_pthread_t.h"
#include <signal.h>
#include <sys/time.h>


#define RUNTIME 25000

int num_threads = 0;
static int hpthreads = 0;//Handling the higher priority threads 
static int lpthreads = 0;//Handling the lower priority threads
int waitingid = 0;
int currentrunningid = -1;
pthread * currentthread = NULL;
pthread * currentlpthread = NULL;
int isThreadRunning = 0;
int currentlpid = -1;

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
static int count = 0;
void scheduler(int sig)
{
	printf("Inside the scheduler function \n");

	//Handling the aging process after we call the signal handler is called 5 times. This is when we chnage the priority of the lower priority threads to 0 so that they will be queued more often.
	if(count == 5)
	{
		int i =0;
		for (i = 0;i < lpthreads ; i++) 
		{
			printf("trying to maintain threads\n pushing up this thread's priority\n");
			(*my_thread_lp_list[i]).priority = 0;
			my_thread_running_list[hpthreads] = my_thread_lp_list[i];			
			hpthreads++;
		}
		count = 0;
	}	
		
	my_pthread_yield();
	
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
	
	//it.it_interval.tv_sec = 0;
 	//it.it_interval.tv_usec = RUNTIME;
 	//it.it_value.tv_sec = 0;
 	//it.it_value.tv_usec = RUNTIME;
 	
 	it.it_value.tv_sec = 0;
	if ((*my_thread_running_list[currentrunningid]).running == 1) {
		it.it_value.tv_usec = (*my_thread_running_list[currentrunningid]).timeslice;
		it.it_interval.tv_usec = (*my_thread_running_list[currentrunningid]).timeslice;
	}
	else {
		if (currentlpid != -1) {
			//printf("using lower thread for interrupt\n");
			it.it_value.tv_usec = (*my_thread_lp_list[currentlpid]).timeslice;
			it.it_interval.tv_usec = (*my_thread_lp_list[currentlpid]).timeslice;
		}
	}
	it.it_interval.tv_sec = 0;
 	
 	
 	setitimer(ITIMER_REAL, &it,NULL);
 	
 	func();
 	
 	if((*my_thread_running_list[currentrunningid]).running == 1) {
    	(*my_thread_running_list[currentrunningid]).active = 0;
	}
	else {
		(*my_thread_lp_list[currentlpid]).active = 0;
	}
 	
 	//(*my_thread_running_list[currentrunningid]).active = 0;
 	
 	//printf("End of function in thread \n");
 	printf("The value of number of running threads in prescheduler is : %d \n",hpthreads);
 	//if(hpthreads > 0)
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
	(*thread).priority = 0; //highest priority
	(*thread).active = 1; //Setting the active flag.
	(*thread).running = 0;//Will set this flag when we set this thread to run.
	(*thread).timeslice = RUNTIME; //Initial time allotted 
	makecontext(&((*thread).mycontext), (void*)&prescheduler,1,(void*)function);
	
	//function();
	++num_threads;
	
	my_thread_list[num_threads] = thread;
	//Adding the newly created thread to the running queue.
	my_thread_running_list[hpthreads] = thread;
	hpthreads++;	
	
	//swapcontext(&curr,&((*thread).mycontext));
	
	printf("Thread created successfully\n");
	 
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
		(*my_thread_running_list[currentrunningid]).running = 1;
		swapcontext(&main_context,&((*currentthread).mycontext));
		(*my_thread_running_list[currentrunningid]).running = 0;
		isThreadRunning = 0;		
		printf("Return from swapcontext \n");
	
		//Here handle the high priority threads and the lower priority threads separately.
		if((*my_thread_running_list[currentrunningid]).active == 0)
		{
			//Since the thread is completed here we will remove the thread from the running list
			//Since the function is done. Remove the thread from the running queue.
 			int c = 0;
 			for(c = currentrunningid; c< hpthreads -1; c++)
 				my_thread_running_list[c] = my_thread_running_list[c+1];
 		
 			//Reduce the number of running threads	
 			hpthreads--;
		}
		
		//Check if there are any threads in the lower priority list. If yes then schedule from the lp queue.
		if(lpthreads > 0)
		{
			currentlpid = (currentlpid + 1) % num_threads;		
			currentlpthread = my_thread_lp_list[currentlpid];
			printf("The threadid of the current lp running thread is : %d \n",(*currentlpthread).threadid);
			isThreadRunning = 1;
			(*my_thread_lp_list[currentrunningid]).running = 1;
			swapcontext(&main_context,&((*currentlpthread).mycontext));
			(*my_thread_lp_list[currentlpid]).running = 0;
			isThreadRunning = 0;		
			printf("Return from swapcontext \n");
	
			//Here handle the high priority threads and the lower priority threads separately.
			if((*my_thread_lp_list[currentlpid]).active == 0)
			{
				//Since the thread is completed here we will remove the thread from the running list
				//Since the function is done. Remove the thread from the running queue.
	 			int c = 0;
	 			for(c = currentlpid; c< lpthreads -1; c++)
	 				my_thread_lp_list[c] = my_thread_lp_list[c+1];
	 		
	 			//Reduce the number of running threads	
	 			lpthreads--;
			}
		}
	
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
		my_thread_waiting_list[currentthread->waitjoin] = NULL;
		my_thread_running_list[hpthreads] =  my_thread_list[currentthread->waitjoin]; 
		hpthreads++;	
	}
	free(currentthread->mycontext.uc_stack.ss_sp);
	free(currentthread);	
	int c = 0;
 	for(c = currentrunningid; c< hpthreads -1; c++)
 		my_thread_running_list[c] = my_thread_running_list[c+1];	
	hpthreads--;
	my_thread_list[currentrunningid] = NULL;	
	num_threads--;
	isThreadRunning = 0;
	my_pthread_yield();
}
int my_pthread_join(pthread *thread,  void **value_ptr){
	printf("thread %d is inside join\n", currentrunningid);
	thread->waitjoin = currentthread->threadid;	
	value_ptr = &(currentthread->status);
	// TODO set currentthread waiting
	//
	my_thread_waiting_list[currentthread->waitjoin] = thread; 
	int c = 0;
	for(c = currentrunningid; c< hpthreads -1; c++)
 		my_thread_running_list[c] = my_thread_running_list[c+1];	
	hpthreads--;
	isThreadRunning = 0;
	my_pthread_yield();
}

void initialize()
{
	printf("Inside the initialize function \n");
	printf("The number of running threads is : %d \n",hpthreads);
	
	while(hpthreads + lpthreads > 0)	
		my_pthread_yield();
	
	printf("Initialization is done \n");
}


