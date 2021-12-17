#include "curve.h"
#include "math.h"
#include "math_custom.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

double* point_curve(pVector c0,int index){
    if(c0 == NULL ){ printf("Error! Return NULL");return NULL;}
    else if(vector_dim(c0)%2 != 0 ){ printf("Error! Curves-vectors must have even number of coordinates\n"); return NULL;}
    else if(index*2 >vector_dim(c0) ){printf("Error! Index out of range !\n"); return NULL;}
    double * point = malloc(sizeof(double)*2);
    point[0] = vector_coord(c0,index);
    point[1] = vector_coord(c0,index+1);
    return point;
}

int delete_point_curve(double** point){
    if(*point == NULL){ return 1;}
    free(*point);
    *point = NULL;
    return 0;
}

double dist_discrete_frechet(pVector p0, pVector p1){
    printf("----1-----\n");
    if(p0 == NULL || p1 == NULL){ printf(" - Error! p0 mor p1 are NULL !\n"); return -1.0; }
    else if( p0 == p1 ){ return 0.0;}
    printf("----2-----\n");

    print_vector_coords(p0);
    print_vector_coords(p1);

    int n = vector_dim(p0)/2;  // size of p0 curve
    int m = vector_dim(p1)/2;  // size of p1 curve

    // initialize cost table
    double c[n][m]; // cost table
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++)
            c[i][j] = -1.0;
    }

    // find first points of each curve
    double* point_p0 = point_curve(p0,0);
    double* point_p1 = point_curve(p1,0);

    // calculate c(0,0)
    c[0][0] = norm_2(point_p0, point_p1);

    // delete first points ponters
    delete_point_curve(&point_p0);
    delete_point_curve(&point_p1);

    // create stacks for c table indexes
    pStack list_i = create_stack();
    pStack list_j = create_stack();

    // pust to stacks n and m of c table
    push_stack(list_i,n-1);
    push_stack(list_j,m-1);

    // discrete frechet iteration
    int i = n-1,j = m-1;
    while( !is_empty_stack(list_i) && i> -1 && j > -1){
        printf("c[%d][%d] = %lf\n",i,j,c[i][j]);
        i = pop_stack(list_i);
        j = pop_stack(list_j);

        if( c[i][j] < 0){
            if( i == 0 && j == 0){
                continue;
            }
            else if( i == 0 && j > 0){
                if( c[i][j-1] > 0){

                    c[i][j] = max( c[i][j-1], norm_2(point_p0,point_p1));

                    delete_point_curve(&point_p0);  delete_point_curve(&point_p1);
                }
                else{
                    push_stack(list_i,i);
                    push_stack(list_j,j);

                    push_stack(list_i,i);
                    push_stack(list_j,j-1);
                    continue;
                }
            }
            else if( i>0 && j == 0){
                if( c[i-1][j] > 0){
                    point_p0 = point_curve(p0,i);   point_p1 = point_curve(p1,j);

                    c[i][j] = max( c[i-1][j], norm_2(point_p0,point_p1));

                    delete_point_curve(&point_p0);  delete_point_curve(&point_p1);
                }
                else{
                    push_stack(list_i,i);
                    push_stack(list_j,j);

                    push_stack(list_i,i-1);
                    push_stack(list_j,j);
                    continue;
                }
            }
            else{ // i>0 and j>0
                if(c[i-1][j] > -1 && c[i-1][j-1] > -1 && c[i][j-1] > -1){
                    point_p0 = point_curve(p0,i);   point_p1 = point_curve(p1,j);

                    c[i][j] = max(min( min( c[i-1][j],c[i-1][j-1] ),c[i][j-1]) , norm_2(point_p0,point_p1));

                    delete_point_curve(&point_p0);  delete_point_curve(&point_p1);
                }
                else{
                    printf("-----\n");

                    push_stack(list_i,i);
                    push_stack(list_j,j);

                    push_stack(list_i,i-1);
                    push_stack(list_j,j);
                    
                    push_stack(list_i,i-1);
                    push_stack(list_j,j-1);

                    push_stack(list_i,i);
                    push_stack(list_j,j-1);

                    printf("list_i -----\n");
                    print_stack(list_i);
                    printf("list_j -----\n");
                    print_stack(list_j);
                    printf("-----\n");
                    continue;
                }
            }
        }
        else{
            // printf("c[%d][%d] = %lf\n",i,j,c[i][j]);
            continue;
        }
    }
    return c[n][m];
}

pVector snapping_vector(pVector p0,pVector t,double delta){
    if(p0 == NULL || t == NULL){ printf(" - Error! p0 or t are NULL !\n"); return NULL; }
    float coords[vector_dim(p0)];
    int num = vector_dim(p0);
    for(int i=0; i< num; i=i+2){
        for( int j=0; j< vector_dim(t); j++){
            coords[i+j] = floor((vector_coord(p0,i+j) - vector_coord(t,j) )/delta+0.5)*delta + vector_coord(t,j);
        }
    }
    return init_vector(vector_id(p0),vector_dim(p0),coords);
}

pVector remove_duplicates_vector(pVector p0,int d,double padding_number){
    if(p0 == NULL){ printf(" - Error! p0 is NULL !\n"); return NULL; }
    float coords[vector_dim(p0)];
    
    float tmp_x = vector_coord(p0,0);
    float tmp_y = vector_coord(p0,1);
    int counter = d;
    coords[0] = vector_coord(p0,0);
    coords[1] = vector_coord(p0,1);


    for( int i=d; i<vector_dim(p0); i+=d){
        if(tmp_x == vector_coord(p0,i) && tmp_y == vector_coord(p0,i+1)){
            printf("same !!\n");
            continue;
        }
        else{
            // copy to new array
            coords[i] = vector_coord(p0,i);
            coords[i+1] = vector_coord(p0,i+1);

            // set tmp_x, tmp_y
            tmp_x = vector_coord(p0,i);
            tmp_y = vector_coord(p0,i+1);
        }
    }
    if( counter < vector_dim(p0)){
        for(int i=counter; i< vector_dim(p0); i++)
            coords[i] = padding_number;
    }

    return init_vector(vector_id(p0),vector_dim(p0),coords);
}