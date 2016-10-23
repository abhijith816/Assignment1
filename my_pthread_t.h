#ifndef MY_PTHREAD_H
#define MY_PTHREAD_H 

#define MEM SIGSTKSZ
#define DEBUG
#define MAX_TCB_NUM 100
#define MAX_JOIN_NUM 10
#define HIGH_PRI 0
#define LOW_PRI 5
#define UNSET 0
#define READY 1
#define BLOCKED 2
typedef unsigned int uint;
typedef uint my_pthread_t;
typedef int my_pthread_attr_t;
typedef struct my_pthread_mutex_t my_pthread_mutex_t;
typedef struct pthread_mutexattr_t{} pthread_mutexattr_t; 

typedef struct my_tcb my_tcb;

int my_pthread_create(my_pthread_t * thread, my_pthread_attr_t * attr, void *(*function)(void*), void* arg);
void my_pthread_yield();
void my_pthread_exit(void* value_ptr); 
int my_pthread_join(my_pthread_t thread, void **value_ptr); 
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const my_pthread_mutexattr_t *mutexattr);
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);
#endif
