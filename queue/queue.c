#include "queue.h"

int push_back(queue* queue,node* node){
	if (queue==NULL){
		return EINVAL;
	}	
	/*Push back*/
	if(is_empty(queue)){
		queue->front=node;
		queue->back=node;
	}else{
		queue->back->next=node;
		node->next=NULL;
	}
	/*Updating record*/
	queue->back=node;
	queue->size++;
	return 0;
}

int pop_front(queue* queue,node** popped_node_ptr){
	/*Not allowing to pop an empty queue*/
	if (is_empty(queue)){
		return EPERM;
	}
	if (queue==NULL){
		return EINVAL;
	}
	/*Pop front*/
	node* old_front=queue->front;
	if(queue->size==1){
		queue->front=NULL;
		queue->back=NULL;
	}else{
		queue->front=old_front->next;
	}
	/*Updating record*/
	queue->size--;
	/*return popped node*/
	*popped_node_ptr=old_front;
	return 0;
}

int is_empty(queue* queue){
	if(queue->size==0){
		return 1;	
	}
	else
		return 0;
}

int queue_init(queue** queue_ptr){
	*queue_ptr=malloc(sizeof(queue));
	(*queue_ptr)->front=NULL;
	(*queue_ptr)->back=NULL;
	(*queue_ptr)->size=0;
}