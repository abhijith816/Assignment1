#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<unistd.h>
#include"MyAllocation.h"

metadata_ptr metadata_list[MAX_THREADS];
static void* MEM_HEAD = NULL;
static int crntThread = 0;

struct _metadata{
    size_t size;
    int free;
    metadata_ptr next;
    char data[1];
};

void* init(){
	void* p = NULL;
    int i;

    printf("Initizlizing Memory of %lf Bytes...\n", MEM_SIZE);
    /*Allocate aligned memory*/
	p = (void*)memalign(sysconf(_SC_PAGE_SIZE), MEM_SIZE);	
    /*Allocate one page for each thread*/
    for(i = 0; i < MAX_THREADS; ++i){
        metadata_list[i] = p + i * PAGE_SIZE ;
        metadata_list[i]->size = PAGE_SIZE - sizeof(metadata);
        metadata_list[i]->free = 1;
        metadata_list[i]->next = NULL;
    } 
    /*The rest memory*/
    metadata_list[i] = p + i * PAGE_SIZE;
    metadata_list[i]->size = MEM_SIZE - MAX_THREADS * PAGE_SIZE;
    metadata_list[i]->free = 1;
    metadata_list[i]->next = NULL;
	return p;
}
void* split(metadata_ptr old_ptr, size_t size){
    metadata_ptr new_ptr;
    new_ptr = (metadata_ptr)(old_ptr->data + size);
    new_ptr->size = old_ptr->size - size - sizeof(metadata);
    new_ptr->free = 1;
    new_ptr->next = old_ptr->next; 
    old_ptr->size = size;
    old_ptr->next = new_ptr; 
    return old_ptr;
}
void* myallocate(size_t size, char* file, int line, int threadReq){
    metadata_ptr ptr;
    if(MEM_HEAD == NULL){// Memory has not been initialized
        MEM_HEAD = init();
    }

    /*Allocating memory for current thread*/
	printf("Allocating size %d Bytes at %s, line %d\n", size, file, line);
    ptr = metadata_list[crntThread];
    while(ptr && !(ptr->free && ptr->size >= size)){
        ptr = ptr-> next;
    } 
    if(ptr != NULL){
        split(ptr, size);
        ptr->free = 0;
    }
    return ptr->data;
}

void mydeallocate(void* data_ptr, char* file, int line, int threadReq){
    metadata_ptr ptr = data_ptr - sizeof(metadata);
    ptr->free = 1; 
	printf("Deallocating %s, line %d\n", file, line);
}
