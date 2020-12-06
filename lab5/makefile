CC = gcc
FLAGS = -Wall -Werror -Wextra -pedantic -g

all: static_link_main1.out static_link_main2.out dynamic_link_main.out

static_link_main1.out: static_link_main.o realization1.o
	${CC} ${FLAGS} static_link_main.o realization1.o -o static_link_main1.out

static_link_main2.out: static_link_main.o realization2.o
	${CC} ${FLAGS} static_link_main.o realization2.o -o static_link_main2.out

static_link_main.o: static_link_main.c
	${CC} ${FLAGS} -c static_link_main.c


realization1.o: realization1.c e_and_gcf.h
	${CC} ${FLAGS} -c -fPIC realization1.c

realization2.o: realization2.c e_and_gcf.h
	${CC} ${FLAGS} -c -fPIC realization2.c


dynamic_link_main.out: dynamic_link_main.o librealization1.so librealization2.so
	${CC} ${FLAGS} -rdynamic -o dynamic_link_main.out dynamic_link_main.o -ldl -lm -lrealization2 -lrealization1

dynamic_link_main.o: dynamic_link_main.c e_and_gcf.h
	${CC} ${FLAGS} -c -fPIC dynamic_link_main.c

librealization1.so: realization1.o e_and_gcf.h
	${CC} ${FLAGS} -shared -o librealization1.so realization1.o -lm 

librealization2.so: realization2.o e_and_gcf.h
	${CC} ${FLAGS} -shared -o librealization2.so realization2.o -lm 

clean:
	rm -rf *.so *.o *.out