#include "vma.h"

#define MAX_STR 256

// Doubly-linked lists definitions
dll_list_t*
dll_create(unsigned int data_size)
{
	// Allocating memory for structure and set variables
    dll_list_t *lista = calloc(1, sizeof(dll_list_t));
    DIE(lista == NULL, "calloc() for list failed");
    lista->head = NULL;
    lista->data_size = data_size;
    lista->size = 0;
    return lista;
}

dll_node_t*
dll_get_nth_node(dll_list_t* list, unsigned int n)
{
	// If list is empty, return null
    if(list->size == 0)
        return NULL;
	// We adjust n so it cycles
	n = (n + 1) % list->size - 1;
    if(n == -1)
		n = list->size - 1;
    // We go to the n-th node
    dll_node_t *nod = list->head;
    while(n > 0) {
        nod = nod->next;
        n--;
    }
    return nod;
}

void
dll_add_nth_node(dll_list_t* list, unsigned int n, const void* new_data)
{
	// n can't go over the list size
	if(n > list->size)
			n = list->size;
	// Allocating the node's and it's data's memory
    dll_node_t *nod = calloc(1, sizeof(dll_node_t));
	DIE(nod == NULL, "calloc() for node failed");
	nod->data = calloc(1, list->data_size);
	DIE(nod->data == NULL, "calloc() for node data failed");
    // Copying data into node
	memcpy(nod->data, new_data, list->data_size);
	// We find where we're supposed to place the new node and we make the connections
	if(list->size == 0) {
		// If this is the first node, we treat it specially so it's doubly linked
        nod->prev = nod;
        nod->next = nod;
        list->head = nod;
    } else {
		if(n == 0) {
			nod->next = list->head;
			nod->prev = list->head->prev;
			list->head = nod;
			nod->next->prev = nod;
			nod->prev->next = nod;
		} else if(n == list->size) {
			nod->next = list->head;
			nod->prev = dll_get_nth_node(list, list->size - 1);
			nod->prev->next = nod;
			nod->next->prev = nod;
		} else {
			// Usual case
			nod->prev = dll_get_nth_node(list, n - 1);
			nod->next = nod->prev->next;
      		nod->prev->next = nod;
			nod->next->prev = nod;
		}
    }
    // We increment the size
    list->size++;
}

dll_node_t*
dll_remove_nth_node(dll_list_t* list, unsigned int n)
{
	dll_node_t *nod;
	if(list->size > 0) {
		// We get the specified node
		if(n + 1 > list->size)
			n = list->size - 1;
		nod = dll_get_nth_node(list, n);
		// We adjust the surrounding nodes' neighbors
		if(n == 0)
			list->head = list->head->next;
		nod->prev->next = nod->next;
		nod->next->prev = nod->prev;
		list->size--;
	} else {
		// In case of an error, we make a dummy node
		nod = calloc(1, sizeof(dll_node_t));
		DIE(nod == NULL, "calloc() for dummy node failed");
		nod->data = calloc(1, list->data_size);
		DIE(nod->data == NULL, "calloc() for dummy node data failed");
	}
	return nod;
}

unsigned int
dll_get_size(dll_list_t* list)
{
    return list->size;
}


// Arenas and blocks definitions
arena_t *alloc_arena(const uint64_t size) {
    arena_t *ar;
    // Alocam adresa pentru arena
    ar = calloc(1, sizeof(arena_t));
    DIE(!ar, "calloc() for arena failed");
    // Alocam adresa pentru lista de blocuri si setam variabilele
    ar->arena_size = size;
    ar->alloc_list = calloc(1, sizeof(dll_list_t));
    DIE(!(ar->alloc_list), "calloc() for arena alloc_list failed");
    // Intoarcem adresa arenei
    return ar;
}

void dealloc_arena(arena_t *arena) {
    // Dealocam lista de blocuri
    free(arena->alloc_list);
    // Dealocam arena
    free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

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