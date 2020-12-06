CC = gcc -g

all: main.out daughter.out

main.out: main.c
	${CC} main.c -o main.out -lpthread -lrt

daughter.out: daughter.c
	${CC} daughter.c -o daughter.out -lpthread -lrt

run:
	rm -rf a.txt
	python tester.py
	./main.out < test0.txt
	diff a.txt test0.t

clean:
	rm -rf *.txt