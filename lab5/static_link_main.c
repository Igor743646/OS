#include <stdio.h>
#include <stdlib.h>

#include "square_and_gcf.h"

int main() {

	short command = 0;
	printf("1 - SinIntegral\n2 - GCF\n\n");
	while(scanf("%hd", &command)>0) {

		if (command == 1) {
			float A,B; float n;
			printf("Введите два числа (границы интегрирования функции y=sin(x)): ");
			scanf("%f%f", &A, &B);
			printf("Введите шаг: ");
			scanf("%f", &n);
			printf("Ответ: %f\n", SinIntegral(A, B, n));	
		} else if (command == 2) {
			int A, B;
			printf("Введите два натуральных числа: ");
			scanf("%d%d", &A, &B);
			printf("Ответ: %d\n", GCF(A, B));	
		} else {
			printf("Неправильно введенная команда\n");
		}
	}

	return 0;
}