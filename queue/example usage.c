#include "queue.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	queue* queue;
	node* popped_node;

	queue_init(&queue);
	printf("%d\n", is_empty(queue));

	for (int i = 0; i < 3; ++i)
	{
		push_back(queue,node_init(i));
	}

	printf("Size:%d,Front:%d,Back:%d\n", queue->size,queue->front->data,queue->back->data);
	pop_front(queue,&popped_node);
	printf("Size:%d,Front:%d,Back:%d\n", queue->size,queue->front->data,queue->back->data);
	pop_front(queue,&popped_node);
	printf("Size:%d,Front:%d,Back:%d\n", queue->size,queue->front->data,queue->back->data);
	pop_front(queue,&popped_node);
	printf("Size:%d\n", queue->size);
	return 0;
}
