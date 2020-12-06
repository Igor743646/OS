CC = gcc

all: main.out daughter.out

main.out: main.c
	${CC} main.c -o main.out

daughter.out: daughter.c
	${CC} daughter.c -o daughter.out

run:
	./main.out

clean:
	rm -rf *.txt