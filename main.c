#include "vma.h"
#define MAX_STR 256

int main(void)
{
	arena_t *arena_main = NULL;
	// TODO: The input has to be redone with strtok
	while (1 == 1) {
		char comanda[MAX_STR], delim[] = " ";
		fgets(comanda, MAX_STR, stdin);
		char *tok = strtok(comanda, delim);
		if (strncmp(tok, "ALLOC_ARENA", 11) == 0) {
			unsigned int dim_arena = 0;
			tok = strtok(NULL, delim);
			dim_arena = strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(2, tok, delim);
				break;
			}
			if (!arena_main)
				arena_main = alloc_arena((uint64_t)dim_arena);
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
			unsigned int adresa, marime;
			tok = strtok(NULL, delim);
			adresa = strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				errorer(0, tok, delim);
				break;
			}
			if (arena_main)
				alloc_block(arena_main, (uint64_t)adresa, (uint64_t)marime);
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
			int adresa, marime;
			tok = strtok(NULL, delim);
			adresa = strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = strtol(tok, NULL, 10);
			// STRTOK CHECK
			tok = strtok(NULL, delim);
			if (tok) {
				printf("Invalid command. Please try again.\n");
				break;
			}
			if (arena_main)
				read(arena_main, (uint64_t)adresa, (uint64_t)marime);
			else
				printf("Invalid command. Please try again.");
			continue;
		}
		if (strncmp(tok, "WRITE", 5) == 0) {
			uint64_t adresa, marime;
			char *data;
			tok = strtok(NULL, delim);
			adresa = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			marime = (uint64_t)strtol(tok, NULL, 10);
			tok = strtok(NULL, delim);
			data = calloc(strlen(tok) + 1, sizeof(char));
			strcpy(data, tok);
			tok = strtok(NULL, delim);
			// STRTOK CHECK
			if (tok) {
				free(data);
				errorer(4, tok, delim);
			} else {
				strcpy(data, tok);
				if (!arena_main) {
					printf("Invalid command. Please try again.");
					continue;
				}
				write(arena_main, adresa, marime, (uint8_t *)data);
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