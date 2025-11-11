CC=gcc
CFLAGS=-Iinclude -Wall
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
BIN=bin/myshell

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f src/*.o $(BIN)
