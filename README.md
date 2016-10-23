# Assignment1
First assignment for CS518 Course in Rutgers

Implementation of a simple version of original pthread library in Linux.


## my_pthread_mutex_t sub-module
This mutex implementaion is built on spinning lock and a test-and-set function provided by gcc compiler.

### How to Use it?
#### Create and Initialize a mutex:
``` c
mutex_var=malloc(sizeof(my_pthread_mutex_t));
my_pthread_mutex_init(mutex_var,NULL);
```
#### Lock and unlock your codes:
```c
	my_pthread_mutex_lock(mutex_var);
  -------Your Codes Here------------
	my_pthread_mutex_unlock(mutex_var);
```
#### Destroy your mutex:
```
my_pthread_mutex_destroy(mutex_var);
```
