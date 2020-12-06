#include "square_and_gcf.h"

float SinIntegral(float A, float B, float e) {
	if (A > B) error_msg("Первый аргумент должен быть меньше второго!");
	if (e <= 0) error_msg("Шаг не может быть меньше или равен нулю!");
	printf("Подсчет интеграла методом трапеций\n");
	float S = 0;
	float x1, x2;
	const unsigned long long n = (B - A) / e;
	for (unsigned long long i = 0; i < n; ++i) {
		x1 = A + i * e;
		x2 = x1 + e;
		S += 0.5 * (x2 - x1) * (sin(x1) + sin(x2));
	}
	return S;
}

int GCF(int A, int B) {
	if (A < 1 || B < 1) error_msg("Аргументы должны быть натуральными числами!");
	printf("Подсчет НОД наивным методом\n");
	int result = A < B ? A : B;
	while (A % result != 0 || B % result != 0) {
		--result;
	}
	return result;
}