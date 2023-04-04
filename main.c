#include "vma.h"

#define MAX_STR 256


int main(void)
{
    arena_t *arena_main = NULL;
    // TODO: The input has to be redone with strtok
    while(1 == 1) {
        char comanda[MAX_STR];
        scanf("%s", comanda);

        if(strncmp(comanda, "ALLOC_ARENA", 11) == 0) {
            uint64_t dim_arena = 0;
            scanf("%", SCNu64, &dim_arena);
            if(arena_main == NULL) {
                arena_main = alloc_arena(dim_arena);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "DEALLOC_ARENA", 13) == 0) {
            if(arena_main != NULL) {
                dealloc_arena(arena_main);
                break;
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "ALLOC_BLOCK", 12) == 0) {
            uint64_t adresa, marime;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            if(arena_main != NULL) {
                alloc_block(arena_main, adresa, marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "FREE_BLOCK", 11) == 0) {
            uint64_t adresa = 0;
            scanf("%", SCNu64, &adresa);
            if(arena_main != NULL) {
                //free_block(arena_main, adresa);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "READ", 4) == 0) {
            uint64_t adresa, marime;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            if(arena_main != NULL) {
                read(arena_main, adresa, marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "WRITE", 5) == 0) {
            uint64_t adresa, marime;
            uint8_t data;
            scanf("%", SCNu64, &adresa);
            scanf("%", SCNu64, &marime);
            scanf("%", SCNu8, &data);
            if(arena_main != NULL) {
                write(arena_main, adresa, marime, data);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(comanda, "PMAP", 4) == 0) {
            if(arena_main != NULL) {
                pmap(arena_main);
            } else {
                printf("Invalid command. Please try again.");
            }
        }
    }
    return 0;
}