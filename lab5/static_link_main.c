#include <stdio.h>
#include <stdlib.h>

#include "e_and_gcf.h"

int main() {

	short command = 0;
	printf("1 - E\n2 - GCF\n\n: ");
	while(scanf("%hd", &command)>0) {

		if (command == 1) {
			int x;
			printf("Введите число: ");
			scanf("%d", &x);
			printf("Ответ: %f\n\n", E(x));	
		} else if (command == 2) {
			int A, B;
			printf("Введите два натуральных числа: ");
			scanf("%d%d", &A, &B);
			printf("Ответ: %d\n\n", GCF(A, B));	
		} else {
			printf("Неправильно введенная команда\n");
		}
		printf(": ");
	}

	return 0;
}