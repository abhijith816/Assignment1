#include <stdlib.h>
#include "my_pthread_mutex_t.h"

int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex){
	if (mutex->flag==1)
	{
		return EBUSY;
	}
	if (mutex->flag==0)
	{
		free(mutex);
		mutex=NULL;
	}
	return 0;
}
