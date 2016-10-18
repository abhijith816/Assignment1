#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<ucontext.h>
#include<assert.h>
#define MEM SIGSTKSZ
#define DEBUG
#define MAX_TCB_NUM 100
#define HIGH_PRI 0
#define LOW_PRI 5
typedef unsigned int uint;
typedef uint my_pthread_t;
typedef ucontext_t my_pthread_attr_t;
typedef struct my_tcb{
    int thread_priority;

    my_pthread_t thread_id;
    ucontext_t thread_context;
}my_tcb;

/*Global variable to indicate current and next running tcb*/
my_tcb* cur_tcb; 
my_tcb* next_tcb;

/*Array that indicates wheather a thread is on, 0 indicates can be create*/
int array_tcb[MAX_TCB_NUM];

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
    printf("%d is asking for yielding", call_tcb->thread_id);
    printf("%d is the next thread running", next_tcb->thread_id);
#endif	
    /*Swap context of current thread and next running thread*/
    swapcontext(&(next_tcb->thread_context), &(call_tcb->thread_context));	
}

/*Exit the current thread*/
void my_pthread_exit(void* value_ptr){
    my_tcb* call_tcb = my_pthread_self();
    my_tcb* next_tcb = my_pthread_next();

    pop_front_RQ(call_tcb);

	/*TODO: Pass the value_ptr to scheduler to relieve waiting queue. Not sure how to do*/


	/*Free tcb space*/
    free(call_tcb->thread_context.uc_stack.ss_sp);
    free(call_tcb);
    /*Set the thread ID allocatable*/
    array_tcb[call_tcb->thread_id] = 0;

    /*Set context to next thread*/
    setcontext(&(next_tcb->thread_context));	
}

int my_pthread_create( my_pthread_t* thread, my_pthread_attr_t* attr, void*(*function)(void*), void* arg){
    int i = 0;
    my_tcb* new_tcb = malloc(sizeof(my_tcb));
    if( new_tcb == NULL){
        printf("Memory Allocation Error");
        return 1;
    }

    /*Allocate an ID for new thread*/
    *thread = i;
    while(array_tcb[i++] == 1 || thread == 0){
        *thread = (uint)i % (MAX_TCB_NUM - 1);
    }/* Allocate scheme can be modified, such as start allocating from running thread_id + 1*/
    new_tcb->thread_id = *thread;

    /*Set highest priority to new thread*/
    new_tcb->thread_priority = HIGH_PRI;

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
        printf("Memory Allocation Error!");
        array_tcb[new_tcb->thread_id] = 0;
        return 1;/*Allocation Error*/
    }	
    makecontext(&(new_tcb->thread_context), *function, 1, arg);

    /*Push current tcb into running queue*/
    push_back_RQ(&new_tcb);
    return 0;
}
