#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[]) {
	int x, value;
	char endln; 
	int flag = 1;

	while ((read(fileno(stdin), &value, sizeof(int))>0) && (read(fileno(stdin), &endln, sizeof(char))>0)) {

		if (flag == 1) {

			int a = 0;
			x = value;
			flag = 0;

			if (write(fileno(stderr), &a, sizeof(int))==-1) {
				printf("Дочерний процесс не смог передать сообщение\n");
				exit(1);
			}

		} else {
			if (value!=0) {

				int a=0;
				
				FILE *f;

				if (f = fopen(argv[0], "a")) {

					if (fprintf(f, "%d\n", x/value)<0) {

						printf("В файл не смогло записаться значение, принудительное завершение программы...\n");
						a=1;

						if (write(fileno(stderr), &a, sizeof(int))==-1) {
							printf("Дочерний процесс не смог передать сообщение\n");
							exit(1);
						}

					} 
					fclose(f);

					if (write(fileno(stderr), &a, sizeof(int))==-1) {
						printf("Дочерний процесс не смог передать сообщение\n");
						exit(1);
					}

				} else {

					printf("Файл не смог открыться, принудительное завершение программы...\n");
					a=1;

					if (write(fileno(stderr), &a, sizeof(int))==-1) {
						printf("Дочерний процесс не смог передать сообщение\n");
						exit(1);
					}

				}

			} else {

				int a=1;
				if (write(fileno(stderr), &a, sizeof(int))==-1) {
					printf("Дочерний процесс не смог передать сообщение\n");
					exit(1);
				}

			}
		}
		if (endln == '\n') {
			flag = 1; 
		}
	}
}