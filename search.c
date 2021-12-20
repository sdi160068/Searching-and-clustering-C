#include <assert.h>
#include "cube.h"
#include "lsh.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

int main(int argc, char* argv[]){
    // C h e c k   f o r   p a r a m e t e r s
    
    // Parameters with default values
    int k = 4;
    int L = 5;
    int M = 10;
    int probes = 2;
    double delta = 1.0;

    // Parameters must have values
    char *input_file    = NULL;
    char *query_file    = NULL;
    char *output_file   = NULL;
    char *algorithm     = NULL;
    char *metric        = NULL;

    int i=0;
    while(i < argc){
        if(*argv[i] == '-'){
            if(argv[i][1] == 'k' && (i+1) < argc){           // k
                k = atoi(argv[i+1]);
                i++;
            }
            else if(strcmp(argv[i],"-algorithm")==0 && (i+1) < argc){       // algorithm
                algorithm = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(algorithm,argv[i+1]);
                i++;
            }
            else if(strcmp(argv[i],"-probes")==0 && (i+1) < argc){       // probes
                probes = atoi(argv[i+1]);
                i++;
            }
            else if(strcmp(argv[i],"-delta")==0 && (i+1) < argc){       // delta
                delta = atoi(argv[i+1]);
                i++;
            }
            else if(strcmp(argv[i],"-metric")==0 && (i+1) < argc){       // metric
                metric = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(metric,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'L' && (i+1) < argc){       // L
                L = atoi(argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'w' && (i+1) < argc){       // w
                change_w(atoi(argv[i+1]));
                i++;
            }
            else if(argv[i][1] == 'i' && (i+1) < argc){       // input file
                input_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(input_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'q' && (i+1) < argc){       // query file
                query_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(query_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'o' && (i+1) < argc){       // output file
                output_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(output_file,argv[i+1]);
                i++;
            }
        }
        i++;
    }

    // C h e c k   f o r   p a r a m e t e r s
    FILE* file;
    int check =0;
    if(input_file  == NULL){ printf(" - Error! You must give an input  file\n"); check = 1; }
    else if(!(file = fopen(input_file,"r"))){ printf(" - Error! %s doesn't exist\n",input_file); check = 1; }
    else { fclose(file); }

    if(output_file == NULL){ printf(" - Error! You must give an output file\n"); check = 1; }

    if(query_file  == NULL){ printf(" - Error! You must give a  query  file\n"); check = 1; }
    else if(!(file = fopen(query_file,"r"))){ printf(" - Error! %s doesn't exist\n",query_file); check = 1; }
    else { fclose(file); }

    if(delta <= 0.0){        printf(" - Error! Delta must be > 0 \n"); check = 1;  }

    char algorithm_index;
    dist_type metric_bit = L2;
    if(algorithm != NULL ){
        // check algorithm
        if (!strcmp(algorithm,"LSH"))           {     algorithm_index = 'L';  }     // LSH
        else if (!strcmp(algorithm,"Hypercube")){     algorithm_index = 'H';  }     // Hypercube
        else if (!strcmp(algorithm,"Frechet"))  {     algorithm_index = 'F';  }     // Frechet
        else{
            printf(" - Error! Algorithm must be 'LSH', 'Hypercube' or 'Frechet'\n"); 
            algorithm_index = 'N';  // None
            check = 1;
        }
        free(algorithm);
        if(algorithm_index == 'H'){
            k=14;
        }
    }
    else{printf(" - Error! You must give an algorithm\n"); check = 1;}
    if(metric != NULL){
        // check metric
        if(algorithm_index == 'F'){
            if(!strcmp(metric,"discrete"))       {  metric_bit = discrete_frechet;   }
            else if(!strcmp(metric,"continuous")){  metric_bit = continuous_frechet; }
            else{   printf(" - Error! Metric must be 'discrete' or 'continuous'\n"); check = 1; }
        }
    }
    else{   printf(" - Error! Metric must be 'discrete' or 'continuous'\n"); check = 1; }

    if(check){ 
        free(input_file);
        free(output_file);
        free(query_file);
        return 1;           // if there are any errors, return 1
    }  
    
    printf("+--------------------------------------------------\n");
    printf("| k             %d\n",k);
    printf("| L             %d\n",L);
    printf("| M             %d\n",M);
    printf("| delta         %lf\n",delta);
    printf("| probes        %d\n",probes);
    printf("| metric        %s\n|\n",metric);
    printf("| input_file    %s\n",input_file);
    printf("| output_file   %s\n",output_file);
    printf("| query_file    %s\n|\n| ",query_file);

    // select algorithm
    switch (algorithm_index){
    case 'L':
        printf("algorithm     LSH\n");
        printf("+--------------------------------------------------\n");
        lsh(k,L,input_file,output_file,query_file);
        break;
    case 'H':
        printf("algorithm     Hypercube\n");
        printf("+--------------------------------------------------\n");
        cube(k,M,probes,input_file,output_file,query_file);
        break;
    case 'F':
        printf("algorithm     Frechet\n");
        printf("+--------------------------------------------------\n");
        lsh_frechet(k,L,delta,input_file,output_file,query_file,metric_bit);
        break;
    default:
        break;
    }

    // free strings
    free(input_file);
    free(output_file);
    free(query_file);
    free(metric);

    return 0;
}