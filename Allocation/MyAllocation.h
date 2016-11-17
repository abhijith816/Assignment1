#ifndef malloc_h
#define malloc_h
#include<stdlib.h>

#define THREADREQ 1
#define MAX_THREADS 10
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define MEM_SIZE 8e6
#define malloc(x) myallocate(x, __FILE__, __LINE__, THREADREQ)
#define free(x) mydeallocate(x, __FILE__, __LINE__, THREADREQ)
typedef struct _metadata metadata;
typedef metadata* metadata_ptr;
void* init();
void* myallocate(size_t x, char* file, int line, int threadReq);
void mydeallocate(void* x, char* file, int line, int threadReq);

#endif
