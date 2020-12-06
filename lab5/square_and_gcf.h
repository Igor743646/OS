#ifndef SQUARE_AND_GCF_H
#define SQUARE_AND_GCF_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define error_msg(msg) \
		do {perror(msg); exit(EXIT_FAILURE);} while(0)

extern float SinIntegral(float A, float B, float e);
extern int GCF(int A, int B);

#endif