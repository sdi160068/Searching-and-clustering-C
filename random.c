#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

double PI = 3.14159265359;

double normal_generator(){	// normal distribution : μ = 0, σ^2 = 1
	double result;
	static double z1 = 0;
	static double z2 = 0;

	double x1 = random_double(0,1);
	double x2 = random_double(0,1);

	if(z2 == 0){
		z1 = sqrt((-2)*log(x1))*cos(2*PI*x2);
		z2 = sqrt((-2)*log(x1))*sin(2*PI*x2);
		result = z1;
	}
	else{
		result = z2;
		z2 = 0;
	}
	return result;
}

int random_int(int num1,int num2){	// returns uniformly random ints , >= min and < max
	init_random_generator();
	int min,max;
	if(num1 < num2){
		min = num1;
		max = num2;
	}
	else{
		min = num2;
		max = num1;
	}
	return rand()%(max+1) + min;
}

double random_double(double num1, double num2){
	init_random_generator();
	double min,max;
	if(num1 < num2){
		min = num1;
		max = num2;
	}
	else{
		min = num2;
		max = num1;
	}
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// https://stackoverflow.com/questions/33058848/generate-a-random-double-between-1-and-1/33059025

void init_random_generator(){
	static bool ok = true;
	if(ok){							// initialize ONLY ONCE the random generator ( srand )
		srand(time(0));
		ok = false;
	}
}

/*
source
https://mathworld.wolfram.com/Box-MullerTransformation.html
*/