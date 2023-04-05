CFLAGS = -fno-PIC -m32 -g -std=c99
LDFLAGS = -no-pie -m32

.DEFAULT_GOAL: all

.PHONY: all clean

all: main

main: main.o vma.o
	$(CC) $(LDFLAGS) -o $@ $^ -lm

main.o: main.c vma.h

vma.o: vma.c

clean:
	rm -f *.o main
	rm vma.o main.o
