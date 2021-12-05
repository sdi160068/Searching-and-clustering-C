 // Christoforos Kladis 1115201600068

#ifndef data_H
#define data_H

#include <stdio.h>

typedef struct data * pData;

char* scan(int *check);

char* scan_space(int *check);

pData scan_full();

char* read_line(FILE* filename,int* check);

pData create_data();

pData return_data(FILE* filename);

void update_data(pData d, char* input);				// adds a string to data array

void update_data_allocate(pData d, char* input);	// copy a string to data array by allocating memory

void print_data(pData p0);

void print_data_no_info(pData p0);					//print the strings from data array without info like the size of the array

void print_data_line(pData p0);

void free_data(pData* p0);

void free_scan();

int data_getSize(pData p0);                // returns the size of table. If p0 == NULL, returns EOF
char* data_getWord(pData p0,int index);    // returns table[index] . If p0 == NULL, index < 0 or index >= size, returns NULL

long int lines_of_file(char* filename);

#endif
