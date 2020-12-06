#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{	
	// fd 0 - для чтения, 1 - для записи
	char *fileName = (char*) malloc(sizeof(char)*256); 

	printf("Введите имя файла: "); 
	scanf("%s", fileName);

	int fd[2]; 
	if (pipe(fd)==-1) {
		printf("Pipe fd не открылся\n");
		exit(0);
	}

	int fd2[2];
	if (pipe(fd2)==-1) {
		printf("Pipe fd2 не открылся\n");
		exit(0);
	}

	int id = fork();
	if (id == -1) 
	{
		perror("fork error");
		return -1;
	}
	else if (id == 0) // дочерний процесс
	{
		close(fd[1]);
		close(fd2[0]);

		if (dup2(fd[0], fileno(stdin))==-1) {
			printf("Не смог создаться дескриптор файла, завершение программы...\n");
			exit(0);
		}

		if (dup2(fd2[1], fileno(stderr))==-1) {
			printf("Не смог создаться дескриптор файла, завершение программы...\n");
			exit(0);
		}

		if (execl("daughter.out", fileName, NULL)==-1) {
			printf("Не смог запустить дочерний процесс в другом файле...\n");
			exit(0);
		}

		close(fd[0]);
		close(fd2[1]);
	}
	else // родительский процесс
	{
		close(fd[0]);
		close(fd2[1]);

		int x, y=0;
		char endln;

		if (scanf("%d%c", &x, &endln)>0) {

			if (write(fd[1], &x, sizeof(int))==-1) {
				printf("Значение не смогло передасться, выход...\n");
				exit(0);
			}

			if (write(fd[1], &endln, sizeof(char))==-1) {
				printf("Значение не смогло передасться, выход...\n");
				exit(0);
			}

			read(fd2[0], &y, sizeof(int));
		}

		while (scanf("%d%c", &x, &endln)>0) {

			if (write(fd[1], &x, sizeof(int))==-1) {
				printf("Значение не смогло передасться, выход...\n");
				exit(0);
			}

			if (write(fd[1], &endln, sizeof(char))==-1) {
				printf("Значение не смогло передасться, выход...\n");
				exit(0);
			}

			read(fd2[0], &y, sizeof(int));

			if (y) {
				break;
			} else {
				y=0;
			}
		}

		close(fd[1]);
		close(fd2[0]);
	}
	return 0;
}

