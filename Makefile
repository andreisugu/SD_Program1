CC=gcc
CFLAGS= -m32 -g -lm -std=c99

TARGETS = main

build: $(TARGETS)

main: main.c vma.o vma.h
	$(CC) $(CFLAGS) -g -o vma vma.o main.c

clean:
	rm -f $(TARGETS)
	rm -f vma.o

.PHONY: pack clean
