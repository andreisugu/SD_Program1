#include "vma.h"

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
    // Allocate address for arena
    ar = calloc(1, sizeof(arena_t));
    DIE(!ar, "calloc() for arena failed");
    // Allocate addres for variables list and adjust initial variables
    ar->arena_size = size;
    ar->alloc_list = calloc(1, sizeof(dll_list_t));
    ar->alloc_list->size = 0;
    ar->alloc_list->data_size = sizeof(block_t);
    DIE(!(ar->alloc_list), "calloc() for arena alloc_list failed");
    // Returning arena address
    return ar;
}

void dealloc_arena(arena_t *arena) {
    // We go trough all the blocks inside the arena
    while(arena->alloc_list->size > 0) {
        dll_node_t *arena_node = dll_remove_nth_node(arena->alloc_list, 0);
        block_t *bl = arena_node->data;
        // We deallocate all the miniblocks inside the block
        for(int j = 0; j < (int)bl->size; j++) {
            dll_list_t *list = bl->miniblock_list;
            while(list->size > 0) {
                dll_node_t *tmp = dll_remove_nth_node(list, 0);
                // Deallocate rw buffer
                miniblock_t *tmp2 = tmp->data;
                free(tmp2->rw_buffer);
                // Deallocate miniblock
                free(tmp2);
                // Deallocate removed node
                free(tmp);
            }
        }
        // The block is freed; We are free to deal with the block
        free(arena_node->data);
        free(arena_node);
    }
    // Deallocating arena and block list
    free(arena->alloc_list);
    free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size) {
    // We verify wether or not we can even allocate the block
    if(address > arena->arena_size) {
        printf("The allocated address is outside the size of arena\n");
        return;
    }
    if(address + size > arena->arena_size) {
        printf("The end address is past the size of the arena\n");
        return;
    }
    // Cautam in lista de blocuri sa vedem daca exista un bloc la adresa primita
    int inters = 0;
    dll_node_t *nod = arena->alloc_list->head;
    uint64_t address_end = address + size;
    dll_node_t *merger_node = NULL, *merger_end_node = NULL;
    dll_node_t *first_favor = NULL, *last_favor = NULL;
    block_t *merger = NULL, *merger_end = NULL; 
    for(int i = 0; i < arena->alloc_list->size; i++) {
        // Pentru usurinta folosirii acestora, notam separat extremitatile adreselor blocului
        uint64_t adr, adr_end;
        block_t *bl = (block_t*)nod->data;
        adr = bl->start_address;
        adr_end = adr + bl->size;
        // Verificam daca adresa noului nostru bloc intersecteaza blocul acesta
        if(!((address_end <= adr) || (address >= adr_end))) {
            // daca address == adr_end, atunci dau merge cat timp nu se intersecteaza cu nimic
            printf("This zone was already allocated.\n");
            return;
        }
        // Verificam daca am gasit un bloc potential pentru merging
        if(address == adr_end) {
            merger_end = bl;
            merger_end_node = nod;
        }
        if(address_end == adr) {
            merger = bl;
            merger_node = nod;
        }
        if(address >= adr_end) {
            last_favor = nod;
        }
        if(address_end <= adr && first_favor == NULL) {
            first_favor = nod;
        }
        // We continue onto the next block
        nod = nod->next;
    }
    // Alocarea blocului si merge-uri
    block_t *bl = calloc(1, sizeof(block_t));
    bl->start_address = address;
    bl->size = size;
    // Merge la capatul stanga al blocului
    if(merger_end != NULL) {
        bl->size = bl->size + merger_end->size;
        bl->start_address = merger_end->start_address;
    }
    if(merger != NULL) {
        bl->size = bl->size + merger->size;
    }
    // Merging la listele de miniblocuri 
    bl->miniblock_list = calloc(1, sizeof(dll_list_t));
    dll_list_t *minib_l = (dll_list_t*)bl->miniblock_list, *copy;
    int i = 0;
    if(merger_end != NULL) {
        copy = merger_end->miniblock_list;
        nod = copy->head;
        while(i < (int)copy->size) {
            dll_add_nth_node(minib_l, i, nod->data);
            nod = nod->next;
            i++;
        }
    }
    int j = 0;
    if(merger != NULL) {
        copy = merger->miniblock_list;
        nod = copy->head;
        while(j < (int)copy->size) {
            dll_add_nth_node(minib_l, i + j, nod->data);
            nod = nod->next;
            j++;
        }
    }
    // Eliminam noduri , dealocam blocuri si introducem noul nod in lista de blocuri
    dll_node_t *f_node = calloc(1, sizeof(dll_node_t));
    f_node->data = bl;
    if(merger_end != NULL) {
        if(merger_end_node == arena->alloc_list->head)
            arena->alloc_list->head = f_node;
        free(merger_end->miniblock_list);
        free(merger_end_node->data);
        f_node->prev = merger_end_node->prev;
        f_node->next = merger_end_node->next;
        free(merger_end_node);
        arena->alloc_list->size--;
    }
    if(merger != NULL) {
        if(merger_node == arena->alloc_list->head)
            arena->alloc_list->head = f_node;
        free(merger->miniblock_list);
        free(merger_node->data);
        if(f_node->prev == NULL)
            f_node->prev = merger_node->prev;
        f_node->next = merger_node->next;
        free(merger_node);
        arena->alloc_list->size--;
    }
    // If this is the first block, we make it point to itself
    if(arena->alloc_list->size == 0) {
        f_node->prev = f_node;
        f_node->next = f_node;
        arena->alloc_list->head = f_node;
        arena->alloc_list->size = 1;
        return;
    }
    // If we didn't merge, we treat separately
    if(f_node->prev == NULL) {
        if(last_favor != NULL)
            f_node->prev = last_favor;
        else
            f_node->prev = arena->alloc_list->head->prev;
    }
    if(f_node->next == NULL) {
        if(first_favor != NULL) {
            f_node->next = first_favor;
            if(first_favor == arena->alloc_list->head)
                arena->alloc_list->head = f_node;
        }
        else
            f_node->next = arena->alloc_list->head;
    }
    // We adjust the next and prev nodes
    f_node->prev->next = f_node;
    f_node->next->prev = f_node;
    arena->alloc_list->size++;
}

//void free_block(arena_t *arena, const uint64_t address);

void pmap(const arena_t *arena) {
    printf("Total memory: 0x%X bytes\n", (unsigned int)arena->arena_size);
    // Total arena free memory, blocks, miniblocks
    int miniblocks = 0;
    uint64_t left = arena->arena_size;
    dll_node_t *nod = arena->alloc_list->head;
    for(int i = 0; i < arena->alloc_list->size; i++) {
        // Free memory
        block_t *block = nod->data;
        uint64_t block_size = block->size;
        left = left - block_size;
        // Miniblocks
        dll_list_t *mb = block->miniblock_list;
        miniblocks = miniblocks + mb->size;
        // Going to the next block
        nod = nod->next;
    }
    printf("Free memory: 0x%X bytes\n", (unsigned int)left);
    printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
    printf("Number of allocated miniblocks: %d\n", miniblocks);
    // Blocks statistics
    nod = arena->alloc_list->head;
    for(int i = 0; i < arena->alloc_list->size; i++) {
        block_t *block = nod->data;
        printf("\nBlock %d begin\n", i);
        printf("Zone: 0x%X - ", (unsigned int)block->start_address);
        printf("0x%X\n", (unsigned int)block->start_address + block->size);
        //TODO: Miniblocks
        printf("Block %d end\n", i);
        nod = nod->next;
    }
}