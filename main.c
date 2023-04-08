#include "vma.h"
#define MAX_STR 1024

int main(void)
{
	arena_t *arena_main = NULL;
	while (1 == 1) {
		char comanda[MAX_STR], delim[] = " ";
		fgets(comanda, MAX_STR, stdin);
		char *tok = strtok(comanda, delim);
		if (strncmp(tok, "ALLOC_ARENA", 11) == 0) {
			uint64_t dim_arena = 0;
			tok = strtok(NULL, delim);
			dim_arena = (uint64_t)strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(2, tok, delim);
				break;
			}
			if (!arena_main)
				arena_main = alloc_arena(dim_arena);
			else
				printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "DEALLOC_ARENA", 13) == 0) {
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok)
				errorer(0, tok, delim);
			if (arena_main) {
				dealloc_arena(arena_main);
				break;
			}
			printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "ALLOC_BLOCK", 12) == 0) {
			uint64_t adresa, marime;
			tok = strtok(NULL, delim);
			adresa = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = (uint64_t)strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(0, tok, delim);
				break;
			}
			if (arena_main)
				alloc_block(arena_main, adresa, marime);
			else
				printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "FREE_BLOCK", 11) == 0) {
			tok = strtok(NULL, delim);
			const uint64_t adresa = (uint64_t)strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(0, tok, delim);
				break;
			}
			if (arena_main)
				free_block(arena_main, adresa);
			else
				printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "READ", 4) == 0) {
			uint64_t adresa, marime;
			tok = strtok(NULL, delim);
			adresa = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = (uint64_t)strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(3, tok, delim);
				break;
			}
			if (arena_main)
				read(arena_main, adresa, marime);
			else
				printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "WRITE", 5) == 0) {
			uint64_t adresa, marime;
			int8_t *data;
			tok = strtok(NULL, delim);
			adresa = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, "\n");
			data = calloc(MAX_STR, sizeof(int8_t));
			if (tok)
				strcpy((char *)data, tok);
			while (strlen((char *)data) < marime - 1) {
				strcat((char *)data, "\n");
				fgets(comanda, MAX_STR, stdin);
				tok = strtok(comanda, "\n");
				if (tok)
					strcat((char *)data, tok);
			}
			if (strlen((char *)data) > marime) {
				tok = strtok((char *)data, delim);
				errorer(3, tok, delim);
				free(data);
				continue;
			} else {
				if (!arena_main) {
					printf("Invalid command. Please try again.");
					continue;
				}
				write(arena_main, adresa, marime, data);
				free(data);
			}
			continue;
		}
		if (strncmp(tok, "PMAP", 4) == 0) {
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(0, tok, delim);
				break;
			}
			if (arena_main)
				pmap(arena_main);
			else
				printf("Invalid command. Please try again.\n");
			continue;
		}
		errorer(0, tok, delim);
	}
	return 0;
}