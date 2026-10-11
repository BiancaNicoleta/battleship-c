CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -g
SRC = src/main.c src/board.c src/ship.c src/game.c
TARGET = battleships

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o src/*.o

.PHONY: all clean
