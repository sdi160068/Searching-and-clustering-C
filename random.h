// Christoforos Kladis 1115201600068

#ifndef __random_H__
#define __random_H__

double random_double(double num1, double num2); // returns uniformly random doubles , >= min and < max
int random_int(int num1,int num2);          // returns uniformly random ints , >= min and < max
double normal_generator();                  // normal distribution : μ = 0, σ^2 = 1
void init_random_generator();               // initialize ONLY ONCE the random generator ( srand )
#endif
