#include <stdlib.h>

#ifndef DATA_TYPE
#define DATA_TYPE int      
#endif

typedef struct node
{
	struct node* next;
	DATA_TYPE data;
}node;

node* node_init(DATA_TYPE data);