CC=gcc
CFLAGS=-DCOMPILER_IS_GCC
SRC=propagate.c main.c
OUTPUT=simulate
all:
	$(CC) -o $(OUTPUT) $(SRC) $(CFLAGS)  -lm
