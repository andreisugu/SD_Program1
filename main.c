#include "vma.h"
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define MAX_STR 256


int main(void)
{
    arena_t *arena_alocata = NULL;
    while(1 == 1) {
        char comanda[MAX_STR];
        scanf("%s", comanda);

        if(strncmp(comanda, "ALLOC_ARENA", 11) == 0) {
            uint64_t dim_arena = 0;
            scanf("%", SCNu64, &dim_arena);
            if(arena_alocata == NULL) {
                arena_alocata = alloc_arena(dim_arena); //ceva e sus la marime
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "DEALLOC_ARENA", 13) == 0) {
            if(arena_alocata != NULL) {
                dealloc_arena(arena_alocata);
                break;
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "ALLOC_BLOCK", 12) == 0) {
            uint64_t adresa, marime;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            if(arena_alocata != NULL) {
                alloc_block(arena_alocata, adresa, marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "FREE_BLOCK", 11) == 0) {
            uint64_t adresa = 0;
            scanf("%", SCNu64, &adresa);
            if(arena_alocata != NULL) {
                free_block(arena_alocata, adresa);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "READ", 4) == 0) {
            uint64_t adresa, marime;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            if(arena_alocata != NULL) {
                read(arena_alocata, adresa, marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "WRITE", 5) == 0) {
            uint64_t adresa, marime;
            uint8_t data;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            scanf("%", SCNu8, &data);
            if(arena_alocata != NULL) {
                write(arena_alocata, adresa, marime, data);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "PMAP", 4) == 0) {
            if(arena_alocata != NULL) {
                pmap(arena_alocata);
            } else {
                printf("Invalid command. Please try again.");
            }
        }
    }
    return 0;
}