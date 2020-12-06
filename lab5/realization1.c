#include "e_and_gcf.h"

float E(const int x) {
	if (x < 1) error_msg("Число должно быть натуральным");
	printf("Подсчет E формулой (1+1/x)^x...\n");
	float res = 1;
	float k = (1 + 1/((float)x));
	for (int i = 0; i < x; ++i) res*=k;
	return res;
}

int GCF(int A, int B) {
	if (A < 1 || B < 1) error_msg("Аргументы должны быть натуральными числами!");
	printf("Подсчет НОД методом Евклида...\n");
	while (A != 0 && B != 0) {
		if (A > B) {
			A %= B;
		} else {
			B %= A;
		}
	}
	return A+B;
}