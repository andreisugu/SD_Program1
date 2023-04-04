#include "vma.h"
#define MAX_STR 256

int main(void)
{
    arena_t *arena_main = NULL;
    // TODO: The input has to be redone with strtok
    while(1 == 1) {
        char comanda[MAX_STR], delim[] = " ";
        fgets(comanda, MAX_STR, stdin);
        char *tok = strtok(comanda, delim);
        if(strncmp(tok, "ALLOC_ARENA", 11) == 0) {
            unsigned int dim_arena = 0;
            tok = strtok(NULL, delim);
            dim_arena = strtol(tok, NULL, 10);
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main == NULL) {
                arena_main = alloc_arena((uint64_t)dim_arena);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "DEALLOC_ARENA", 13) == 0) {
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                dealloc_arena(arena_main);
                break;
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "ALLOC_BLOCK", 12) == 0) {
            unsigned int adresa, marime;
            tok = strtok(NULL, delim);
            adresa = strtol(tok, NULL, 10);
            tok = strtok(NULL, delim);
            marime = strtol(tok, NULL, 10);
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                alloc_block(arena_main, (uint64_t)adresa, (uint64_t)marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "FREE_BLOCK", 11) == 0) {
            unsigned int adresa = 0;
            tok = strtok(NULL, delim);
            adresa = strtol(tok, NULL, 10);
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                //free_block(arena_main, (uint64_t)adresa);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "READ", 4) == 0) {
            int adresa, marime;
            tok = strtok(NULL, delim);
            adresa = strtol(tok, NULL, 10);
            tok = strtok(NULL, delim);
            marime = strtol(tok, NULL, 10);
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                read(arena_main, (uint64_t)adresa, (uint64_t)marime);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "WRITE", 5) == 0) {
            unsigned int adresa, marime;
            char *data;
            tok = strtok(NULL, delim);
            adresa = strtol(tok, NULL, 10);
            tok = strtok(NULL, delim);
            marime = strtol(tok, NULL, 10);
            tok = strtok(NULL, delim);
            strcpy(data, tok);
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                write(arena_main, (uint64_t)adresa, (uint64_t)marime, (uint8_t*)data);
            } else {
                printf("Invalid command. Please try again.");
            }
        } else if(strncmp(tok, "PMAP", 4) == 0) {
            // STRTOK CHECK
            tok = strtok(NULL, delim);
            if(tok != NULL) {
                printf("Invalid command. Please try again.\n");
                break;
            }
            if(arena_main != NULL) {
                pmap(arena_main);
            } else {
                printf("Invalid command. Please try again.\n");
            }
        } else {
            printf("Invalid command. Please try again.\n");
        }
    }
    return 0;
}