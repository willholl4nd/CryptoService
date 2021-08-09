CC = gcc
CFLAGS = -Wall -Wextra -g
LIB = -lcurl -ljson-c -lmariadb
SRC = src
OBJ = obj
BIN = bin

TARGET = main

BINS = $(patsubst %,${BIN}/%,${TARGET})
OBJS = $(patsubst %,${OBJ}/%.o,${TARGET})
SRCS = $(patsubst %,${SRC}/%.c,${TARGET})

${BINS}: ${OBJS}
	${CC} ${CFLAGS} $^ -o $@ ${LIB}

${OBJS}: ${SRCS}
	${CC} ${CFLAGS} $^ -c -o $@ 

val: ${BINS}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$^ 
	
clean:
	rm ${OBJS} ${BINS}
