// A stack struct
// Christoforos Kladis 1115201600068

#ifndef __stack__
#define __stack__

#include <stdbool.h>

typedef struct stack * pStack;
typedef struct snode * pSnode;

pStack create_stack();

pSnode init_snode(const int id);

void push_stack(pStack ps,const int id);

int pop_stack(pStack ps);

bool is_empty_stack(pStack ps);

pSnode search_snode(pStack ps,const int id);

void delete_snode(pStack ps,const int id);

void delete_stack(pStack* pps); // You must give the address of stack to work. It returns NULL.

void print_snode(pSnode p0); // prints "node of stack with id" if the node exists, else it prints "there is no node"

void print_stack(pStack ps);

#endif
