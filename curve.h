#ifndef __curve_H__
#define __curve_H__

#include "vector.h"


double* point_curve(pVector c0,int index);
int delete_point_curve(double** point);

double dist_discrete_frechet(pVector p0, pVector p1);

pVector snapping_vector(pVector p0,pVector t,double delta);     // snapping : floor((x-t)/delta + 1/2)*delta +t
pVector remove_duplicates_vector(pVector p0,int d,double padding_number);     // remove duplicates and add pudding number 

#endif