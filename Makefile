CFLAGS = -Wall -Wextra -pedantic
LIBS = -lraylib -lm

.PHONY: all compile

all: compile

compile:
	gcc $(CFLAGS) -o main.out main.c $(LIBS)
