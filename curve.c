#include "curve.h"
#include "math.h"
#include "math_custom.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

double* point_curve(pVector c0,int index, double* point){
    if(c0 == NULL ){ printf("Error! Return NULL");return NULL;}
    else if(vector_dim(c0)%2 != 0 ){ printf("Error! Curves-vectors must have even number of coordinates\n"); return NULL;}
    else if(index*2 >vector_dim(c0) ){printf("Error! Index out of range !\n"); return NULL;}
    int index_t = index*2;
    point[0] = vector_coord(c0,index_t);
    point[1] = vector_coord(c0,index_t+1);
    return point;
}

void print_point(double* p){
    printf("(%lf,%lf)\n",p[0],p[1]);
}

int delete_point_curve(double** point){
    if(*point == NULL){ return 1;}
    free(*point);
    *point = NULL;
    return 0;
}

// double dist_discrete_frechet(pVector p0, pVector p1){
//     if(p0 == NULL || p1 == NULL){ printf(" - Error! p0 mor p1 are NULL !\n"); return -1.0; }
//     else if( p0 == p1 ){ return 0.0;}

//     int n = vector_dim(p0)/2;  // size of p0 curve
//     int m = vector_dim(p1)/2;  // size of p1 curve

//     // initialize cost table
//     double c[n][m]; // cost table
//     for(int i=0; i<n; i++){
//         for(int j=0; j<m; j++)
//             c[i][j] = -1.0;
//     }

//     // find first points of each curve
//     double point_p0[2];
//     point_curve(p0,0,point_p0);
//     double point_p1[2];
//     point_curve(p1,0,point_p0);

//     // calculate c(0,0)
//     c[0][0] = norm_2(point_p0, point_p1);

//     // create stacks for c table indexes
//     pStack list_i = create_stack();
//     pStack list_j = create_stack();

//     // pust to stacks n and m of c table
//     push_stack(list_i,n-1);
//     push_stack(list_j,m-1);

//     // discrete frechet iteration
//     int i = n-1,j = m-1;
//     while( !is_empty_stack(list_i) && i> -1 && j > -1){double dist_discrete_frechet(pVector p0, pVector p1){
//     if(p0 == NULL || p1 == NULL){ printf(" - Error! p0 mor p1 are NULL !\n"); return -1.0; }
//     else if( p0 == p1 ){ return 0.0;}

//     int n = vector_dim(p0)/2;  // size of p0 curve
//     int m = vector_dim(p1)/2;  // size of p1 curve

//     // initialize cost table
//     double c[n][m]; // cost table
//     for(int i=0; i<n; i++){
//         for(int j=0; j<m; j++)
//             c[i][j] = -1.0;
//     }

//     // find first points of each curve
//     double point_p0[2];
//     point_curve(p0,0,point_p0);
//     double point_p1[2];
//     point_curve(p1,0,point_p0);

//     // calculate c(0,0)
//     c[0][0] = norm_2(point_p0, point_p1);

//     // create stacks for c table indexes
//     pStack list_i = create_stack();
//     pStack list_j = create_stack();

//     // pust to stacks n and m of c table
//     push_stack(list_i,n-1);
//     push_stack(list_j,m-1);

//     // discrete frechet iteration
//     int i = n-1,j = m-1;
//     while( !is_empty_stack(list_i) && i> -1 && j > -1){
//         i = pop_stack(list_i);
//         j = pop_stack(list_j);

//         if( c[i][j] < 0){
//             if( i == 0 && j > 0){
//                 if( c[i][j-1] > 0){
//                     c[i][j] = max( c[i][j-1], norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{
//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i);
//                     push_stack(list_j,j-1);
//                     continue;
//                 }
//             }
//             else if( i>0 && j == 0){
//                 if( c[i-1][j] > 0){
//                     c[i][j] = max( c[i-1][j], norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{
//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j);
//                     continue;
//                 }
//             }
//             else{ // i>0 and j>0
//                 if(c[i-1][j] > -1 && c[i-1][j-1] > -1 && c[i][j-1] > -1){
//                     c[i][j] = max( min(c[i][j-1] , min( c[i-1][j],c[i-1][j-1] ) ) , norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{

//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j);
                    
//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j-1);

//                     push_stack(list_i,i);
//                     push_stack(list_j,j-1);
//                     continue;
//                 }
//             }
//         }
//         else{
//             continue;
//         }
//     }

//     delete_stack(&list_i);
//     delete_stack(&list_j);
//     return c[n-1][m-1];
// }
//         i = pop_stack(list_i);
//         j = pop_stack(list_j);

//         if( c[i][j] < 0){
//             if( i == 0 && j > 0){
//                 if( c[i][j-1] > 0){
//                     c[i][j] = max( c[i][j-1], norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{
//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i);
//                     push_stack(list_j,j-1);
//                     continue;
//                 }
//             }
//             else if( i>0 && j == 0){
//                 if( c[i-1][j] > 0){
//                     c[i][j] = max( c[i-1][j], norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{
//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j);
//                     continue;
//                 }
//             }
//             else{ // i>0 and j>0
//                 if(c[i-1][j] > -1 && c[i-1][j-1] > -1 && c[i][j-1] > -1){
//                     c[i][j] = max( min(c[i][j-1] , min( c[i-1][j],c[i-1][j-1] ) ) , norm_2(point_curve(p0,i,point_p0) , point_curve(p1,j,point_p1)));
//                 }
//                 else{

//                     push_stack(list_i,i);
//                     push_stack(list_j,j);

//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j);
                    
//                     push_stack(list_i,i-1);
//                     push_stack(list_j,j-1);

//                     push_stack(list_i,i);
//                     push_stack(list_j,j-1);
//                     continue;
//                 }
//             }
//         }
//         else{
//             continue;
//         }
//     }

//     delete_stack(&list_i);
//     delete_stack(&list_j);
//     return c[n-1][m-1];
// }

double dist_frechet(pVector p0, pVector p1){
    int size0 = vector_dim(p0)/2;
    int size1 = vector_dim(p1)/2;
    double c[size0][size1];
    int n=1,m=1;
    double point0[2] , point1[2];


    c[0][0] =  norm_2(point_curve(p0,0,point0),point_curve(p1,0,point1));

    for(int i=1; i<size0; i++){
        c[i][0] = max(c[i-1][0], norm_2(point_curve(p0,i,point0) , point_curve(p1,0,point1)));
    }
    for(int i=1; i<size1; i++){
        c[0][i] = max(c[0][i-1], norm_2(point_curve(p0,0,point0) , point_curve(p1,i,point1)));
    }

    n = 1; m = 1;
    while( n < size0 && m < size1){
        for(int i=n; i<size0; i++)
            c[i][m] = max(min(min(c[i-1][m],c[i][m-1]),c[i-1][m-1]), norm_2(point_curve(p0,i,point0),point_curve(p1,m,point1)));
        for(int i=m; i<size1; i++)
            c[n][i] = max(min(min(c[n-1][i],c[n][i-1]),c[n-1][i-1]), norm_2(point_curve(p0,n,point0),point_curve(p1,i,point1)));
        n++;
        m++;
    }
    return c[size0-1][size1-1];
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