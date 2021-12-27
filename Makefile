CC=g++
CFLAGS=-std=gnu++20 -Wall

.PHONY: all clean

all: ecs_test

ecs_test: src/main.cpp
	$(CC) $(CFLAGS) $^ -g -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@rm -rf */*.o ecs_test ecs_test.dSYM/