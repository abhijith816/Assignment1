#include "my_pthread_mutex_t.h"

int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	__sync_synchronize();
	mutex->flag=0;
	return 0;
}
