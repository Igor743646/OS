#include <dlfcn.h>
#include "square_and_gcf.h"

#define error_msg(msg) \
		do {perror(msg); exit(EXIT_FAILURE);} while(0)

int main() {

	short flag = 0;

	void* current_library = dlopen("./librealization1.so", RTLD_LAZY);
	if (!current_library) error_msg("Динамическая библиотека не смогла открыться");

	float (*SinIntegral) (float, float, float);
	int (*GCF) (int, int);

	*(void**) (&SinIntegral) = dlsym(current_library, "SinIntegral");
	*(void**) (&GCF) = dlsym(current_library, "GCF");
	if (dlerror()) error_msg("Ошибка загрузки функций");

	short command = 0;
	printf("0 - Change library\n1 - SinIntegral\n2 - GCF\n\n: ");
	while(scanf("%hd", &command)>0) {

		if (command == 0) {
			dlclose(current_library);
			if (flag) {
				current_library = dlopen("./librealization1.so", RTLD_LAZY);
				if (!current_library) error_msg("Динамическая библиотека не смогла открыться");
				flag = 0;
			} else {
				current_library = dlopen("./librealization2.so", RTLD_LAZY);
				if (!current_library) error_msg("Динамическая библиотека не смогла открыться");
				flag = 1;
			}
			*(void**) (&SinIntegral) = dlsym(current_library, "SinIntegral");
			*(void**) (&GCF) = dlsym(current_library, "GCF");
			if (dlerror()) error_msg("Ошибка загрузки функций");
			printf("Загрузка прошла успешно\n\n");
		} else if (command == 1) {
			float A,B, n;
			printf("Введите два числа (границы интегрирования функции y=sin(x)): ");
			scanf("%f%f", &A, &B);
			printf("Введите шаг: ");
			scanf("%f", &n);
			printf("Ответ: %f\n\n", SinIntegral(A, B, n));	
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

	dlclose(current_library);
	return 0;
}