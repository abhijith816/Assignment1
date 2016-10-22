#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
struct QueueRecord{
	int Capacity;
	int Front;
	int Rear;
	int Size;
	ElementType *Array;
};
int IsEmpty( Queue Q ){
	return Q->Size == 0;
}
int IsFull( Queue Q ){
	return Q->Size == Q->Capacity;
}
Queue CreateQueue( int MaxElements ){
	Queue Q = malloc( sizeof( struct QueueRecord) );
	if( Q == NULL)
		printf( "Out of Space\n" );
	Q->Array = malloc( sizeof( ElementType ) * MaxElements );
	if( Q->Array == NULL )
		printf( "Out of Space\n" );
	Q->Capacity = MaxElements;
	MakeEmpty( Q );
	return Q;
}
void DisposeQueue( Queue Q ){
	if( Q != NULL ){
		free( Q->Array );
		free( Q );
	}
}
void MakeEmpty( Queue Q ){
	Q->Size = 0;
	Q->Front = 1;
	Q->Rear = 0;
}

static int succ( int Value, Queue Q ){
	if( ++Value == Q->Capacity )
		Value = 0;
	return Value;
}

void Enqueue( ElementType x, Queue Q ){
	if( IsFull( Q ) ){
		printf( "Queue is full\n" );
	}
	else{
		Q->Rear = succ( Q->Rear, Q );
		Q->Array[Q->Rear] = x;
		Q->Size++;
	}
		
}
void Dequeue( Queue Q ){
	if( IsEmpty( Q ) )
		printf( "Queue is empty\n" );
	else{
		Q->Front = succ( Q->Front, Q );
		Q->Size--;
	}
}
ElementType FrontAndDequeue( Queue Q ){
	ElementType x;
	if( IsEmpty( Q ) )
		printf( "Queue is empty\n" );
	else{
		x = Q->Array[Q->Front];
		Q->Front = succ( Q->Front, Q );
		Q->Size--;
	}
	return x;
}
