#include "e_and_gcf.h"

float E(const int x) {
	if (x < 1) error_msg("Число должно быть натуральным");
	printf("Подсчет E суммой ряда...\n");
	float res = 1;
	float k = 1;
	for (int i = 1; i <= x; ++i) {
		k*=i;
		res+=1/k;
	}
	return res;
}

int GCF(const int A, const int B) {
	if (A < 1 || B < 1) error_msg("Аргументы должны быть натуральными числами!");
	printf("Подсчет НОД наивным методом\n");
	int result = A < B ? A : B;
	while (A % result != 0 || B % result != 0) {
		--result;
	}
	return result;
}