CC = g++
CFLAGS = -Wall -Wextra -g -std=c++17
LIB = -lcurl -ljson-c -lmariadb -lmgl
SRC = src
OBJ = obj
BIN = bin

TARGET = bin/main
FILES = main curl email json mysql graph 

OBJS = $(patsubst %,%.o,${FILES})
OBJECTS = $(patsubst %,${OBJ}/%.o,${FILES})
SRCS = $(wildcard ${SRC}/*.cpp)

EMAIL = test

.PHONY:all

all: $(OBJS) $(TARGET)

${OBJS}: %.o: $(SRC)/%.cpp
	${CC} ${CFLAGS} -c $< -o $(OBJ)/$@

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} $(OBJECTS) -o $(TARGET) ${LIB}

val: ${BINS}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$^ ${EMAIL}
	
clean:
	rm ${OBJECTS} ${TARGET}
