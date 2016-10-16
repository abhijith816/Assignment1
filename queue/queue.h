/*A C queue library using linked list*/
/*Modify DATA_TYPE as u want in node.h*/
#include <stdlib.h>
#include "node.h"
#include "errno.h"

typedef struct queue
{
	node* front;
	node* back;
	int size;
}queue;

int push_back(queue* queue,node* node);
int pop_front(queue* queue,node** popped_node);
int is_empty(queue* queue);
int queue_init(queue** queue); //Initialize an empty queue