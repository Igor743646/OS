CC = gcc
FLAGS = -std=c17 -Wall -Wextra -Werror -pedantic

all: main.out

main.out: main.c matfunc.h
	${CC} ${FLAGS} main.c -lpthread -o main.out

run:
	./main.out 1 < test0.txt
	sleep 3
	./main.out 2 < test0.txt
	sleep 3
	./main.out 3 < test0.txt
	sleep 3
	./main.out 4 < test0.txt
	sleep 3
	./main.out 5 < test0.txt
