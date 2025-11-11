CC = gcc
CFLAGS = -Wall -g -Iinclude

SRC = src/main_builtins.c src/shell.c src/execute.c
OBJ = $(SRC:.c=.o)
BIN = bin/myshell

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

