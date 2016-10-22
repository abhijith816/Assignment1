#ifndef _Queue_h
#define _Queue_h
#include"my_pthread_t.h"

struct QueueRecord;
typedef struct QueueRecord *Queue;
typedef my_tcb* ElementType;

int IsEmpty( Queue Q );
int IsFull( Queue Q );
Queue CreateQueue( int MaxElements );
void DisposeQueue( Queue Q );
void MakeEmpty( Queue Q );
void Enqueue( ElementType x, Queue Q );
void Dequeue( Queue Q );
ElementType FrontAndDequeue( Queue );

#endif
