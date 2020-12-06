#include "square_and_gcf.h"

float SinIntegral(float A, float B, float e) {
	if (A > B) error_msg("Первый аргумент должен быть меньше второго!");
	if (e <= 0) error_msg("Шаг не может быть меньше или равен нулю!");
	printf("Подсчет интеграла методом прямоугольников...\n");
	float S = 0;
	const unsigned long long  n = (B - A) / e;
	for (unsigned long long i = 0; i < n; ++i) {
		S += sin(A + i * e) * e;
	}
	return S;
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