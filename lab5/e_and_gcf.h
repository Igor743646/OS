#ifndef E_AND_GCF_H
#define E_AND_GCF_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define error_msg(msg) \
		do {perror(msg); exit(EXIT_FAILURE);} while(0)

float E(const int x);
int GCF(int A, int B);

#endif