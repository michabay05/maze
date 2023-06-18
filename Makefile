CFLAGS = -Wall -Wextra -pedantic

.PHONY: all compile

all: compile

compile:
	gcc $(CFLAGS) -o main.out main.c
