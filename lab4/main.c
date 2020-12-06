/*
	Глушатов Игорь Сергеевич. Группа М80-207Б-19.
	Лабораторная работа №4. File mapping.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define BAD_SHM_OPEN -1
#define and &&
#define or ||
#define error_msg_with_free_fileName(msg) \
		do {free(fileName); perror(msg); exit(EXIT_FAILURE);} while(0)

static const char* mmaped_file_for_numbers = "p1";
static const char* mmaped_file_for_flag = "p2";
static const char* mmaped_mutex_one = "mu1";
static const char* mmaped_mutex_two = "mu2";

void mutexInit(pthread_mutex_t *mt) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mt, &attr);
}

int main() {

	char* fileName = (char*) malloc(sizeof(char)*256); 

	printf("Введите имя файла: ");
	if (!(scanf("%s", fileName)>0)) error_msg_with_free_fileName("bad scanf fileName");

	int *fd1; // для передачи введенного числа
	int *fd2; // флаг завершения программы
	pthread_mutex_t *m1; // два мьютекса для синхронизации родительского и дочернего процессов
	pthread_mutex_t *m2;

	// открытие дескрипторов для передачи сообщений и синхронизации процессов
	// shm_open возвращает номер дескриптора по типу stdin==1
	int pipe1 = shm_open(mmaped_file_for_numbers, O_RDWR | O_CREAT, S_IRWXU);
	int pipe2 = shm_open(mmaped_file_for_flag, O_RDWR | O_CREAT, S_IRWXU);
	int mutex1 = shm_open(mmaped_mutex_one, O_RDWR | O_CREAT, S_IRWXU);
	int mutex2 = shm_open(mmaped_mutex_two, O_RDWR | O_CREAT, S_IRWXU);

	if (pipe1==BAD_SHM_OPEN or pipe2==BAD_SHM_OPEN) error_msg_with_free_fileName("pipes");
	if (mutex1==BAD_SHM_OPEN or mutex2==BAD_SHM_OPEN) error_msg_with_free_fileName("mutexes");

	// установка длины дескрипторов
	ftruncate(pipe1, sizeof(int)*2);
	ftruncate(pipe2, sizeof(int));
	ftruncate(mutex1, sizeof(pthread_mutex_t));
	ftruncate(mutex2, sizeof(pthread_mutex_t));

	// при ошибки системного вызова ftruncate в errno устанавливается номер ошибки
	if (errno != 0) error_msg_with_free_fileName("bad ftruncate"); 

	// отображение дескрипторов в память
	fd1 = (int *) mmap(NULL, sizeof(int)*2, PROT_READ | PROT_WRITE, MAP_SHARED, pipe1, 0);
	fd2 = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, pipe2, 0);
	m1 = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex1, 0);
	m2 = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex2, 0);

	// при ошибке mmap возвращает константу MAP_FAILED
	if (fd1 == MAP_FAILED or fd2 == MAP_FAILED or m1 == MAP_FAILED or m2 == MAP_FAILED) 
		error_msg_with_free_fileName("map failed");

	// инициализация мьютексов через функцию mutexInit
	mutexInit(m1);
	mutexInit(m2);

	pthread_mutex_lock(m1);
	pthread_mutex_lock(m2);

	int id = fork();
	if (id == -1) {

		error_msg_with_free_fileName("fork error");
		
	} else if (id == 0) { // дочерний процесс

		if (execl("daughter.out", fileName, mmaped_file_for_numbers, \
			mmaped_file_for_flag, mmaped_mutex_one, mmaped_mutex_two, NULL)==-1) \
			error_msg_with_free_fileName("bad execl");
		
	} else { // родительский процесс 

		int x;
		char endln;
		while (scanf("%d%c", &x, &endln)>0) {
	
			fd1[0] = x;
			fd1[1] = (int) endln;

			pthread_mutex_unlock(m1);
			pthread_mutex_lock(m2);

			if (fd2[0]) {
				free(fileName);
				exit(EXIT_SUCCESS);
			} 
		}
	}

	free(fileName);
	return 0;
}