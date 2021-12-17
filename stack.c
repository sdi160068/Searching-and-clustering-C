// A stack struct
// Christoforos Kladis 1115201600068

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct snode{
    int id;
    pSnode next;
    pSnode prev;
}snode;

typedef struct stack{
    int number;
    pSnode first;
    pSnode last;
}stack;

pStack  create_stack(){
    pStack ps = malloc(sizeof(stack));
    ps->first = NULL;
    ps->last = NULL;
    ps->number = 0;
    return ps;
}

pSnode init_snode(const int id){
    pSnode psn = malloc(sizeof(snode));
    psn->id = id;
    psn->next = NULL;
    psn->prev = NULL;
    return psn;
}

void push_stack(pStack ps,const int id){
    if(ps == NULL){printf("- Queue is NULL\n"); return;}
    pSnode psn = init_snode(id);
    if(ps->last == NULL){
        ps->last = psn;
        ps->first = psn;
    }
    else{
        ps->first->prev = psn;
        psn->next = ps->first;
    }
    ps->first = psn;
    ps->number++;
}

int pop_stack(pStack ps){
    if(ps == NULL){printf("- Queue is NULL\n"); return -1;}
    int res;
    if(is_empty_stack(ps)){ return -1;}
    else { res = ps->first->id;}
    delete_snode(ps,res);
    return res;
}

bool is_empty_stack(pStack ps){
    if(ps == NULL){printf("- Queue is NULL\n"); return 1;}
    if(ps->number == 0){ return 1;}
    else{ return 0;}
}

pSnode search_snode(pStack ps,const int id){
    if(ps == NULL){printf("- Queue is NULL\n"); return NULL;}
    pSnode p0 = ps->first;
    while(p0!=NULL){
        if(p0->id == id)
            break;
        p0 = p0->next;
    }
    if (p0==NULL){
        printf("There is no node with id : %d\n",id);
        return NULL;
    }
    return p0;
}

void delete_snode(pStack ps,const int id){
    if(ps == NULL){printf("- Queue is NULL\n"); return;}
    pSnode p0 = search_snode(ps,id);
    if(p0 != NULL){
        if(ps->first == ps->last){
            ps->first = NULL;
            ps->last = NULL;
        }
        else if(ps->first == p0){
            ps->first = p0->next;
            ps->first->prev = NULL;
        }
        else if(ps->last == p0){
            ps->last = p0->prev;
            ps->last->next = NULL;
        }
        else{
            p0->prev->next = p0->next;
            p0->next->prev = p0->prev;
        }
        free(p0);
        p0 = NULL;
        ps->number-=1;
    }
}

void delete_stack(pStack* pps){
    if(*pps == NULL){printf("- Queue is NULL\n"); return;}
    pSnode p0 = (*pps)->first;
    while(p0 != NULL){
        delete_snode((*pps),p0->id);
        p0 = (*pps)->first;
    }
    free((*pps));
    *pps = NULL;
}

void print_snode(pSnode p0){
    if(p0 == NULL)
        printf("there is no node\n");
    else
        printf("node of stack with id : %d\n",p0->id);
}

void print_stack(pStack ps){
    pSnode p0 = ps->first;
    while( p0 != NULL){
        print_snode(p0);
        p0 = p0->next;
    }
}
