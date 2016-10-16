#include "my_pthread_mutex_t.h"

int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	mutex->flag=0;
	return 0;
}
