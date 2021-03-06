#include "my_pthread_t.h"
#include "my_pthread_mutex_t.h"

int my_pthread_mutex_lock(my_pthread_mutex_t *mutex){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	while(__sync_lock_test_and_set(&(mutex->flag), 1))
	my_pthread_yield();
	return 0;
}
