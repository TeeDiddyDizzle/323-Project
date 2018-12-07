# Define required macros here
SHELL = /bin/sh
OBJS =  main.o 
CFLAGS = -Wall -g -std=c++14 -DLINUX
CC = g++
INCLUDEs =
LIBS = 

compiler:${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} ${LIBS} -o $@ ${OBJS}

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<