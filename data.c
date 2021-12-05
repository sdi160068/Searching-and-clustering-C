#include "data.h"
#include <dirent.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//----Struct----------------------------
typedef struct data{
    char** t;
    int i;
}data;
//----Struct----------------------------

char* scan(int* check){
    unsigned int len = 20;
    unsigned int current_size = len;
    char c;

    c = getchar();
    if(c==EOF){ return NULL; }
    int i=0;
    while(c==' '){ c = getchar();}

    if(c == '\n'){ (*check)=0; return NULL;}

    char* str = malloc(len*sizeof(char));

    while(c!=EOF && c!='\n' && c!=' '){
        str[i] = c;
        i++;
        c = getchar();
        if(i==(current_size)){
            current_size += len;
            char * str_help = (char*)realloc(str,sizeof(char)*(current_size));
            if(str_help == NULL)
                printf("- error! Something wrong with realloc in scan function -> data.c!!\n");
            else
                str = str_help;
        }
    }
    char* new_str = malloc(sizeof(char)*(i+1));
    for(int k=0;k<i;k++)
        new_str[k] = str[k];
    new_str[i] = '\0';
    free(str);
    if(c == '\n')
        (*check)=0;
    if(c == EOF)
        (*check)=-1;
    return new_str;
}

char* scan_space(int *check){
    unsigned int len = 20;
    unsigned int current_size = len;
    char c;

    c = getchar();
    if(c==EOF){ return NULL; }
    int i=0;
    while(c==' '){ c = getchar();}

    if(c == '\n'){ (*check)=0; return NULL;}

    char* str = malloc(len*sizeof(char));

    while(c!=EOF && c!='\n'){
        str[i] = c;
        i++;
        c = getchar();
        if(i==(current_size)){
            current_size += len;
            char * str_help = (char*)realloc(str,sizeof(char)*(current_size));
            if(str_help == NULL)
                printf("- error! Something wrong with realloc in scan function -> data.c!!\n");
            else
                str = str_help;
        }
    }
    char* new_str = malloc(sizeof(char)*(i+1));
    for(int k=0;k<i;k++)
        new_str[k] = str[k];
    new_str[i] = '\0';
    free(str);
    if(c == '\n')
        (*check)=0;
    if(c == EOF)
        (*check)=-1;
    return new_str;
}

pData scan_full(){
    pData p0 = create_data();
    int check=1;
    while(check!=0)
        update_data(p0,scan(&check));
    return p0;
}

char* read_line(FILE* filename,int* check){    // returns a word from an file with name filename
    unsigned int len = 10;
    unsigned int current_size = len;

    char* str,c;
    c = fgetc(filename);
    if(c==EOF){ return NULL; }
    int i=0;
    while(c==' ' || c=='\r'){ c = fgetc(filename); }

    if(c == '\n'){ (*check)=0; return NULL;}
    
    str = malloc(len*sizeof(char));
    while(c!=EOF && c!='\n' && c!=' ' && c!='\r'){
        str[i] = c;
        i++;
        c = fgetc(filename);
        if(i==current_size){
            current_size += len;
            char * str_help = (char*)realloc(str,sizeof(char)*(current_size));
            if(str_help == NULL)
                printf("- error! Something wrong with realloc in scan function -> data.c!!\n");
            else{
                str = str_help;
            }
        }
    }
    char* new_str = malloc(sizeof(char)*(i+1));
    for(int k=0;k<i;k++)
        new_str[k] = str[k];
    new_str[i] = '\0';
    free(str);
    if(c == '\n')
        (*check)=0;
    return new_str;
}

pData create_data(){
    pData d = malloc(sizeof(data));
    d->i=0;
    d->t = NULL;
    return d;
}

pData return_data(FILE* filename){
    int check=1;
    char*str;
    str = read_line(filename,&check);
    if(str== NULL)
        return NULL;
    pData p0 = create_data();
    while(str != NULL && check){
        update_data(p0,str);
        str = read_line(filename,&check);
    }
    if(str != NULL)
        update_data(p0,str);
    return p0;
}

void free_data(pData* p0){
    if((*p0) == NULL)
        return;
    for(int j=0; j<(*p0)->i; j++)
        free((*p0)->t[j]);
    free((*p0)->t);
    free((*p0));
    (*p0) = NULL;
}

void free_scan(){
    int check_input=1;
    char* str = scan(&check_input);
    while(check_input!=0 && str!=NULL){free(str); str=scan(&check_input);}free(str);
}

void update_data(pData d, char* input){
    if(input == NULL)
        return;
    d->i++;
    if(d->t == NULL){
        d->t = malloc(sizeof(char*));
        d->t[0] = input;
    }
    else{
        char** t = realloc(d->t,sizeof(char*)*d->i);
        t[d->i-1] = input;
        if(t != NULL)
            d->t = t;
        else{
            d->i--;
            perror("- error! Something wrong with update_data!!\n");
        }
    }
}

void update_data_allocate(pData d, char* input){
    d->i++;
    if(d->t == NULL){
        d->t = malloc(sizeof(char*));
        d->t[0] = malloc(sizeof(char)*(strlen(input)+1));
        strcpy(d->t[0],input);
    }
    else{
        char** t = realloc(d->t,sizeof(char*)*d->i);
        t[d->i-1] = malloc(sizeof(char)*(strlen(input)+1));
        strcpy(t[d->i-1],input);
        if(t != NULL)
            d->t = t;
        else{
            d->i--;
            perror("- error! Something wrong with update_data!!\n");
        }
    }
}

void print_data(pData p0){
    if(p0 == NULL)
        printf("---\n- There is no pData for printing\n---\n");
    else{
        printf("---\ndata->i : %d\n",p0->i);
        for(int i=0; i<p0->i; i++)
            printf("data->[%d] : %s\n",i,p0->t[i]);
        printf("---\n");
    }
}

void print_data_no_info(pData p0){
    if(p0 != NULL){
        for(int i=0; i<p0->i; i++)
            printf("%s\n",p0->t[i]);
    }
}

void print_data_line(pData p0){
    if(p0 != NULL){
        for(int i=0; i<p0->i; i++)
            printf("%s ",p0->t[i]);
        printf("\n");
    }
}

int data_getSize(pData p0){
    if(p0 == NULL){ return EOF;}
    return p0->i;
}

char* data_getWord(pData p0,int index){
    if(p0 == NULL){ return NULL;}
    else if(p0->i < index+1 || index < 0){ return NULL; }
    return p0->t[index];
}

long int lines_of_file(char* filename){
    FILE *fp;
    fp=fopen(filename, "r");
    long int lines =0;

    if ( fp == NULL ) {
    return -1;
    }

    while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
        ++lines;

    fclose(fp);
    return lines;
}