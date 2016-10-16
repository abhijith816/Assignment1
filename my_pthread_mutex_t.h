#include <errno.h>

/*mutex implemented using simple test-and-set spinning lock*/
typedef struct my_pthread_mutex_t
{
	int flag;
} my_pthread_mutex_t;

typedef struct pthread_mutexattr_t{} pthread_mutexattr_t;

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);
