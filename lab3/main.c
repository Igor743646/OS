#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "matfunc.h"
#define FILTR_DIM 3

struct Int {
	int id;
};

static struct Matrix in; // входная матрица для обработки
static struct Matrix out; // обработанная матрица фильтром
static struct Matrix window; // фильтр

void* convolution(void* args) {
	
	struct Int *id = (struct Int*) args;
	int x, y; 
	int xborder = 0; 
	int yborder = 0;

	x = id->id/in.dimm;
	y = id->id%in.dimm;
	if (in.dimn>=2 && in.dimm>=2) {
		if ((x==0 && y==0) || (x==0 && y==in.dimm-1) || (x==in.dimn-1 && y==0) || (x==in.dimn-1 && y==in.dimm-1)) {

			if (x!=0) xborder = FILTR_DIM-1;
			if (y!=0) yborder = FILTR_DIM-1;

			for (int i=0; i<FILTR_DIM*FILTR_DIM; ++i) {
				if (i/FILTR_DIM==xborder || i%FILTR_DIM==yborder) continue;
				out.matr[x][y] += in.matr[x+i/FILTR_DIM-1][y+i%FILTR_DIM-1]*window.matr[i/FILTR_DIM][i%FILTR_DIM];
			} 

		} else if (x==0 || x==in.dimn-1) {

			xborder = (x + FILTR_DIM - in.dimn)*(x>0);
			for (int i=0; i<FILTR_DIM*FILTR_DIM; ++i) {
				if (i/FILTR_DIM==xborder) continue;
				out.matr[x][y] += in.matr[x+i/FILTR_DIM-1][y+i%FILTR_DIM-1]*window.matr[i/FILTR_DIM][i%FILTR_DIM];
			} 
			
		} else if (y==0 || y==in.dimm-1) {

			yborder = (y + FILTR_DIM - in.dimm)*(y>0);
			for (int i=0; i<FILTR_DIM*FILTR_DIM; ++i) {
				if (i%FILTR_DIM==yborder) continue;
				out.matr[x][y] += in.matr[x+i/FILTR_DIM-1][y+i%FILTR_DIM-1]*window.matr[i/FILTR_DIM][i%FILTR_DIM];
			} 

		} else {

			for (int i=0; i<FILTR_DIM*FILTR_DIM; ++i) {
				out.matr[x][y] += in.matr[x+i/FILTR_DIM-1][y+i%FILTR_DIM-1]*window.matr[i/FILTR_DIM][i%FILTR_DIM];
			}

		}
	} else {
		if (in.dimn==1 && in.dimm>=2) {
			if (y==0) {
				out.matr[x][y] = in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2] + in.matr[x][y+1]*window.matr[FILTR_DIM/2][FILTR_DIM/2+1];
			} else if (y==in.dimm-1) {
				out.matr[x][y] = in.matr[x][y-1]*window.matr[FILTR_DIM/2][FILTR_DIM/2-1] + in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2];
			} else {
				out.matr[x][y] = in.matr[x][y-1]*window.matr[FILTR_DIM/2][FILTR_DIM/2-1] + in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2] + in.matr[x][y+1]*window.matr[FILTR_DIM/2][FILTR_DIM/2+1];
			}
		} else if (in.dimn>=2 && in.dimm==1) {
			if (x==0) {
				out.matr[x][y] = in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2] + in.matr[x+1][y]*window.matr[FILTR_DIM/2+1][FILTR_DIM/2];
			} else if (x==in.dimn-1) {
				out.matr[x][y] = in.matr[x-1][y]*window.matr[FILTR_DIM/2-1][FILTR_DIM/2] + in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2];
			} else {
				out.matr[x][y] = in.matr[x-1][y]*window.matr[FILTR_DIM/2-1][FILTR_DIM/2] + in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2] + in.matr[x+1][y]*window.matr[FILTR_DIM/2+1][FILTR_DIM/2];
			}
		} else {
			out.matr[x][y] = in.matr[x][y]*window.matr[FILTR_DIM/2][FILTR_DIM/2];
		}
	}

	return NULL;
}

int main(int argc, char* argv[]) {
	int max_threads = 1;
	if (argc > 1 && atoi(argv[1]) > 0) {
		max_threads = atoi(argv[1]);
	}
 
	int k;
	unsigned int n;
	unsigned int m;

	scanf("%d", &k);
	scanf("%u", &n);
	scanf("%u", &m);
	
	printf("Ввод матрицы...\n");
	scanfMatrix(&in, n, m);
	scanfMatrix(&window, FILTR_DIM, FILTR_DIM);
	scanfWithFillMatrix(&out, n, m, 0);

	printf("Входная матрица:"); 
	printMatrix(&in);
	printf("Фильтр: "); 
	printMatrix(&window);

	time_t start = time(NULL);

	struct Int *args = (struct Int*) malloc(sizeof(struct Int)*(n*m));
	for (unsigned int i=0; i<n*m; ++i) {
		args[i].id = i;
	}

	pthread_t *threads = (pthread_t*) malloc(sizeof(pthread_t)*max_threads);

	for (int i=0; i<k; ++i) {

		if (in.dimm < 10) {
			printf("Промежуточный результат №%d", i+1);
			printMatrix(&in);
		} else {
			printf("Шаг №%d\n", i+1);
		}

		fillMatrix(&out, 0); // Результатная матрица должна быть в начале заполнена нулями

		for (unsigned int t=0; t<n*m; t+=max_threads) { 
			//printf("Номер обрабатываемого пикселя: %d\n", t);
			for (int j=0; j<max_threads; ++j) {
				//printf("Номер работуещего потока: %d\nНомер пикселя: %d\n", j, t+j);
				if (t+j>=n*m) break;
				if (pthread_create(&threads[j], NULL, convolution, (void*) &args[t+j]) != 0) {
					perror("Поток не создался. Завершение программы...");
					return -1;
				}
			}
		
			for (int j=0; j<max_threads; ++j) {
				if ((unsigned int)(t+j)>=n*m) break;
		        if (pthread_join(threads[j], NULL)) {
		        	perror("Поток не завершился. Завершение программы...");
					return -1;
		        }
	 	    }
		}

		swap(&in, &out);
	}
	
	swap(&in, &out);

	free(threads);
	free(args);

	time_t end = time(NULL);

	printf("Конечный результат:");
	printMatrix(&out);

	printf("%ld %ld %ld\n", end-start, start, end);

	FILE *file = fopen("log.txt", "a");
	fprintf(file, "%ld %ld %ld\n", end-start, start, end);
	fclose(file);

	return 0;
}