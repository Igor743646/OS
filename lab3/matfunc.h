#ifndef MATFUNC_H
#define MATFUNC_H
	
struct Matrix {
	int dimn;
	int dimm;
	float **matr;
};

void scanfMatrix(struct Matrix *a, int n, int m) { // Ввод матрицы
	a->dimn = n;
	a->dimm = m;
 	a->matr = (float**)malloc(sizeof(float*)*n);
	for (int i=0; i<n; i++) {
		a->matr[i] = (float*) malloc(sizeof(float)*m);
	}

	for (int i=0; i<n; i++) {
		for (int j=0; j<m; j++) {
			scanf("%f", &a->matr[i][j]);
		}
	}
}

void scanfWithFillMatrix(struct Matrix *a, int n, int m, int num) { // Ввод с заполнением матрицы
	a->dimn = n;
	a->dimm = m;
	a->matr = (float**)malloc(sizeof(float*)*n);
	for (int i=0; i<n; i++) {
		a->matr[i] = (float*) malloc(sizeof(float)*m);
	}

	for (int i=0; i<n; i++) {
		for (int j=0; j<m; j++) {
			a->matr[i][j] = num;
		}
	}
}

void printMatrix(struct Matrix *a) { // Печать матрицы
	printf("\n");
	for (int i=0; i<a->dimn; i++) {
		for (int j=0; j<a->dimm; j++) {
			printf("%.2f ", a->matr[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void fillMatrix(struct Matrix *a, int num) { // Заполнение матрицы
	for (int i=0; i<a->dimn; i++) {
		for (int j=0; j<a->dimm; j++) {
			a->matr[i][j] = num;
		}
	}
}

void swap (struct Matrix *a, struct Matrix *b) { // Обмен ссылками на матрицы одинаковой размерности
	struct Matrix p; float **pp;
	p = *a; pp = a->matr;
	*a = *b; a->matr = b->matr;
	*b = p; b->matr = pp;
}

#endif