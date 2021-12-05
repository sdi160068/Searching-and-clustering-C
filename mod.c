#include <stdio.h>
#include "mod.h"

long int mod(long int a, long int b){
	return (a % b + b) %b;
}