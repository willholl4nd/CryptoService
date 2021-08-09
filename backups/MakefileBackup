CC = gcc
CFLAGS = -Wall -Wextra -g
LIB = -lcurl -ljson-c -lmariadb

main: main.o
	${CC} ${CFLAGS} main.o -o main ${LIB}

main.o: main.c
	${CC} ${CFLAGS} main.c -c 

val: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main
	
clean:
	rm main.o main
