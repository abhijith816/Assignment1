/// In this file maintain all the information that is only absolutely necessary else the preprocessor load will increase.

//Need a list of running and waiting queue. When a thread is created it has to be present in the running queue.

#include <ucontext.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define MEM 64000
#define MAX_THREADS 10


typedef struct {
	int threadid;
	ucontext_t mycontext;
	void * stack;
	int priority;
	float timeslice;
	clock_t starttime;
	int active;
	int waitjoin;// Waitting thread id;
	void * status;
	int running;
	
}pthread;

typedef struct my_pthread_mutex_t
{
	int flag;
} my_pthread_mutex_t;

typedef struct pthread_mutexattr_t{} pthread_mutexattr_t;

extern int finished;
extern int numthreads;
extern int runningid;
extern int waitingid;

static ucontext_t main_context;

//Maintaining an array of pthreads so that we can keep track of them. 
static pthread * my_thread_list[MAX_THREADS];

//Running queue
static pthread * my_thread_running_list[MAX_THREADS];
static pthread * my_thread_lp_list[MAX_THREADS];
static pthread * nl[2];

//Waiting queue
static pthread * my_thread_waiting_list[MAX_THREADS];

extern void init();

/* Declaring all the functions that we are going to use in here*/

//Function to create a thread for a given function given as argument. This function returns 0 on success and on error returns the error id.
extern int my_phread_create(pthread *thread,  void (*function)(void), void * arg);

//Call to scheduler to swap the current thread
extern void my_pthread_yield();

//End the thred that calls this function . If the argument is not null then the return value from the thread will be saved.
extern void pthread_exit(void **value_ptr);

/*******mutex****/
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);


//Call to the thread lib to make sure that the calling thread will not execute until the one it references exits. If the value_ptr is not null then return value of the exiting thread will be passed back.
//extern int my_pthread_join(pthread_t thread, void **value_ptr);
