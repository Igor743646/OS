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
#define error_msg(msg) \
		do {perror(msg); exit(EXIT_FAILURE);} while(0)

void write_in_file(char* file_name, int x, int *fd2, pthread_mutex_t *m2) {
	FILE *f;
	f = fopen(file_name, "a");
	if (f == NULL) {
		fd2[0] = 1;
		pthread_mutex_unlock(m2);
		error_msg("Файл не смог открыться, принудительное завершение программы...");
	}
	if (fprintf(f, "%d\n", x)<0) {
		fd2[0] = 1;
		pthread_mutex_unlock(m2);
		error_msg("В файл не смогло записаться значение, принудительное завершение программы...");
	}
	fclose(f);
}

int main(int argc, void* argv[]) {
	int x, value;
	char endln; 
	int flag = 1;

	const char* mmaped_file_for_numbers = (char*)argv[1];
	const char* mmaped_file_for_flag = (char*)argv[2];
	const char* mmaped_mutex_one = (char*)argv[3];
	const char* mmaped_mutex_two = (char*)argv[4];

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

	if (pipe1==BAD_SHM_OPEN or pipe2==BAD_SHM_OPEN) error_msg("pipes");
	if (mutex1==BAD_SHM_OPEN or mutex2==BAD_SHM_OPEN) error_msg("mutexes");

	// установка длины дескрипторов
	ftruncate(pipe1, sizeof(int)*2);
	ftruncate(pipe2, sizeof(int));
	ftruncate(mutex1, sizeof(pthread_mutex_t));
	ftruncate(mutex2, sizeof(pthread_mutex_t));

	// при ошибки системного вызова ftruncate в errno устанавливается номер ошибки
	if (errno != 0) error_msg("bad ftruncate"); 

	// отображение дескрипторов в память
	fd1 = (int *) mmap(NULL, sizeof(int)*2, PROT_READ | PROT_WRITE, MAP_SHARED, pipe1, 0);
	fd2 = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, pipe2, 0);
	m1 = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex1, 0);
	m2 = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex2, 0);

	while (1) {
		
		pthread_mutex_lock(m1);

		value = fd1[0];
		endln = (char)fd1[1];

		if (flag == 1) {
			x = value;
			flag = 0;
			fd2[0] = 0;
		} else {
			if (value!=0) {
				x/=value;
				fd2[0] = 0;
			} else {
				fd2[0] = 1;
				pthread_mutex_unlock(m2);
				exit(EXIT_SUCCESS);
			}
		}

		if (endln == '\n') {
			write_in_file(argv[0], x, fd2, m2);
			flag = 1; 
		}

		pthread_mutex_unlock(m2);
	}
}