#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<ucontext.h>
#include<assert.h>
#include"my_pthread_t.h"
#include"Queue.h"
struct my_tcb{
	int thread_priority;
	void* retVal;

	Queue waitForJoin;
	my_pthread_t thread_id;
	ucontext_t thread_context;
};
void func_1();
/*Global variable to indicate current and next running tcb*/
my_tcb* cur_tcb; 
my_tcb* next_tcb;

/*Array that indicates thread status, 0 indicate no such thread, 1 indicate running , 2 indicates blocked*/
int status_tcb[MAX_TCB_NUM];
my_tcb* all_tcb[MAX_TCB_NUM];
/*Running Queue and Waiting Queue*/
Queue rq = NULL;
Queue wq = NULL;

/************************************************/
/*Return the current running thread tcb*/
my_tcb* my_pthread_self(){
	assert(cur_tcb != NULL);
	return cur_tcb;
}
/*Return the next thread scheduled to run*/
my_tcb* my_pthread_next(){
	assert(next_tcb != NULL);
	return next_tcb;
}

/*Yield to other threads*/
void my_pthread_yield(){
	my_tcb* call_tcb = my_pthread_self();
	my_tcb* next_tcb = my_pthread_next();
#ifdef DEBUG
	printf("%d is asking for yielding\n", call_tcb->thread_id);
	printf("%d is the next thread running\n", next_tcb->thread_id);
#endif	
		
	/*Swap context of current thread and next running thread*/
	swapcontext(&(call_tcb->thread_context), &(next_tcb->thread_context));	
}

/*Join another thread*/
int my_pthread_join(my_pthread_t thread, void**value_ptr){
	my_tcb* call_tcb = my_pthread_self();
	my_tcb* next_tcb = my_pthread_next();

#ifdef DEBUG
	printf("thread %d is asking for joinng thread %d\n", call_tcb->thread_id, thread);
#endif
	if(value_ptr != NULL){
		call_tcb->retVal = *value_ptr;
	}		
	Enqueue(call_tcb, all_tcb[thread]->waitForJoin);
	status_tcb[call_tcb->thread_id] = BLOCKED;
	my_pthread_yield();	
	return 0;
}

/*Exit the current thread*/
void my_pthread_exit(void* value_ptr){
	my_tcb* call_tcb = my_pthread_self();
	my_tcb* next_tcb = my_pthread_next();
	my_tcb* wait_tcb = NULL;
	/*Pass value_ptr to waiting threas*/
	while(!IsEmpty(call_tcb->waitForJoin)){
		wait_tcb = FrontAndDequeue(call_tcb->waitForJoin);
		wait_tcb->retVal = value_ptr;
		status_tcb[wait_tcb->thread_id] = READY;
	}

	/*Free tcb space*/
	free(call_tcb->thread_context.uc_stack.ss_sp);
	free(call_tcb);
	/*Set the thread ID allocatable*/
	status_tcb[call_tcb->thread_id] = UNSET;
	all_tcb[call_tcb->thread_id] = NULL;
	/*Set context to next thread*/
	setcontext(&(next_tcb->thread_context));	
}

int my_pthread_create( my_pthread_t* thread, my_pthread_attr_t* attr, void*(*function)(void*), void* arg){
	int i = 0;
	my_tcb* new_tcb = malloc(sizeof(my_tcb));
	if( new_tcb == NULL){
		printf("Memory Allocation Error\n");
		return 1;
	}

	/*Allocate an ID for new thread*/
	while(status_tcb[i] != UNSET){ 
		i = (i + 1) % MAX_TCB_NUM;
	}
	*thread = i;
	new_tcb->thread_id = *thread;

	/*Set highest priority to new thread*/
	new_tcb->thread_priority = HIGH_PRI;

	/*Initialize waiting for join queue*/
	new_tcb->waitForJoin = CreateQueue(MAX_JOIN_NUM);

	/*Create context for new thread*/
	getcontext(&(new_tcb->thread_context));
	new_tcb->thread_context.uc_link = 0;
    new_tcb->thread_context.uc_stack.ss_sp = malloc(MEM);
    new_tcb->thread_context.uc_stack.ss_size = MEM;
    new_tcb->thread_context.uc_stack.ss_flags = 0;
#ifdef DEBUG
    printf("creating context for thread %d\n", new_tcb-> thread_id);
#endif
    if (new_tcb->thread_context.uc_stack.ss_sp == NULL){
        printf("Memory Allocation Error!\n");
		status_tcb[new_tcb->thread_id] = 0;
        return 1;/*Allocation Error*/
    }	
    makecontext(&(new_tcb->thread_context), &func_1, 1, arg);
	/*Set new thread status to running*/
	status_tcb[new_tcb->thread_id] = READY;
	/*Push new tcb into all_tcb array*/
	all_tcb[new_tcb->thread_id] = new_tcb;
	/*Push current tcb into running queue*/
	if(rq == NULL){
		rq = CreateQueue(MAX_TCB_NUM);
	}
	Enqueue(new_tcb, rq);
    return 0;
}

void func_1(){
	printf("This is func_1\n");
	my_pthread_exit(NULL);	
}
int main(int argc, char* argv[]){
	my_pthread_t thread1, thread2;
	ucontext_t main_context, context_1, context_2;
	int flag = 0;

	printf("This is in main\n");
	my_pthread_create(&thread1, NULL, &func_1, NULL);
	cur_tcb = malloc(sizeof(my_tcb));
	cur_tcb->thread_id = 10;
	getcontext(&(cur_tcb->thread_context)); 
	next_tcb = all_tcb[thread1];
	if(next_tcb != NULL) my_pthread_yield();	
	return 0;
} 
