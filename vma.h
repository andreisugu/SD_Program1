#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, " (%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

// Doubly-linked list definitions
typedef struct dll_node_t dll_node_t;
struct dll_node_t {
	void *data;
	dll_node_t *prev, *next;
};

typedef struct {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int size;
} dll_list_t;

dll_list_t *dll_create(unsigned int data_size);

dll_node_t *dll_get_nth_node(dll_list_t *list, unsigned int n);

void dll_add_nth_node(dll_list_t *list, unsigned int n, const void *new_data);

dll_node_t *dll_remove_nth_node(dll_list_t *list, unsigned int n);

unsigned int dll_get_size(dll_list_t *list);

// Arenas and Blocks definitions
typedef struct {
	uint64_t start_address;
	size_t size;
	void *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	dll_list_t *alloc_list;
} arena_t;

arena_t *alloc_arena(const uint64_t size);  //DONE
void dealloc_arena(arena_t *arena);  //DONE

void alloc_block
(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write
(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

// Extra
void sim_errorer(int num);
void errorer(int num, char *tok, const char *delim);
void fix_my_block(block_t *block);