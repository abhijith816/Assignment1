#include <stdlib.h>
#include "node.h"

#ifndef DATA_TYPE
#define DATA_TYPE int
#endif

node* node_init(DATA_TYPE data){
	node* node=malloc(sizeof(node));
	node->data=data;
	return node;
}