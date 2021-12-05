#include <stdio.h>
#include "loading.h"

const int check_play = 1;
static int check = 1;
static int step;
static int loop_size;
static char* ltext;
static int count;
static int i;

int init_loading(char* text,int size){
    if(check && check_play && text != NULL){
        ltext = text;
        step = size/100-1;
        loop_size = size;
        i = 0;
        check = 0;
        count = 0;
        printf("%s   0%%",text);
        return 0;
    }
    return 1;
}

int loading(){
    if(!check_play || check){ return 1;}
    else if(count == 100){
        return 0;
    }
    else if(step <= 0){
        count+= 100/loop_size;
        fflush(stdout);
        printf("\r%s %3d%%",ltext,count);
        i = 0;
    }
    else if(i == step){
        count++;
        fflush(stdout);
        printf("\r%s %3d%%",ltext,count);
        i = 0;
    }
    else if(i<step)
        i++;
    return 1;
}

void end_loading(){
    if(check == 0 && check_play){
        check = 1;
        fflush(stdout);
        printf("\r%s 100%%\n",ltext);
    }
}