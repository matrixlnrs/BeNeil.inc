# Variables
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
TARGET = beneil

# Règle par défaut
all: $(TARGET)

$(TARGET): beneil.o
	$(CC) $(CFLAGS) -o $(TARGET) beneil.o

beneil.o: beneil.c
	$(CC) $(CFLAGS) -c beneil.c

run: all
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

re: clean all

.PHONY: all run clean re