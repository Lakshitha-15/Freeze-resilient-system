CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -pthread

all: freeze

freeze: main.o runtime/container.o
	$(CC) $^ -o freeze

main.o: main.c runtime/container.h
	$(CC) $(CFLAGS) -c main.c -o main.o

runtime/container.o: runtime/container.c runtime/container.h
	$(CC) $(CFLAGS) -c runtime/container.c -o runtime/container.o

clean:
	rm -f freeze *.o runtime/*.o

.PHONY: all clean
