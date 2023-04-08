#include "vma.h"

// Doubly-linked lists definitions
dll_list_t *
dll_create(unsigned int data_size)
{
	// Allocating memory for structure and set variables
	dll_list_t *lista = calloc(1, sizeof(dll_list_t));
	DIE(!lista, "calloc() for list failed");
	lista->head = NULL;
	lista->data_size = data_size;
	lista->size = 0;
	return lista;
}

dll_node_t *
dll_get_nth_node(dll_list_t *list, unsigned int n)
{
	// If list is empty, return null
	if (list->size == 0)
		return NULL;
	// We adjust n so it cycles
	n = (n + 1) % list->size - 1;
	if (n == (unsigned int)(-1))
		n = list->size - 1;
	// We go to the n-th node
	dll_node_t *nod = list->head;
	while (n > 0) {
		nod = nod->next;
		n--;
	}
	return nod;
}

void
dll_add_nth_node(dll_list_t *list, unsigned int n, const void *new_data)
{
	// n can't go over the list size
	if (n > list->size)
		n = list->size;
	// Allocating the node's and it's data's memory
	dll_node_t *nod = calloc(1, sizeof(dll_node_t));
	DIE(!nod, "calloc() for node failed");
	nod->data = calloc(1, list->data_size);
	DIE(!nod->data, "calloc() for node data failed");
	// Copying data into node
	memcpy(nod->data, new_data, list->data_size);
	if (list->size == 0) {
		// If this is the first node, we treat it specially
		nod->prev = nod;
		nod->next = nod;
		list->head = nod;
	} else {
		if (n == 0) {
			nod->next = list->head;
			nod->prev = list->head->prev;
			list->head = nod;
			nod->next->prev = nod;
			nod->prev->next = nod;
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

dll_node_t *
dll_remove_nth_node(dll_list_t *list, unsigned int n)
{
	dll_node_t *nod;
	if (list->size > 0) {
		// We get the specified node
		if (n + 1 > list->size)
			n = list->size - 1;
		nod = dll_get_nth_node(list, n);
		// We adjust the surrounding nodes' neighbors
		if (n == 0)
			list->head = list->head->next;
		nod->prev->next = nod->next;
		nod->next->prev = nod->prev;
		list->size--;
	} else {
		// In case of an error, we make a dummy node
		nod = calloc(1, sizeof(dll_node_t));
		DIE(!nod, "calloc() for dummy node failed");
		nod->data = calloc(1, list->data_size);
		DIE(!nod->data, "calloc() for dummy node data failed");
	}
	return nod;
}

unsigned int
dll_get_size(dll_list_t *list)
{
	return list->size;
}

// Arenas and blocks definitions
arena_t *alloc_arena(const uint64_t size)
{
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

void dealloc_arena(arena_t *arena)
{
	// We free the list of blocks
	while (arena->alloc_list->size > 0) {
		dll_node_t *block_node = dll_remove_nth_node(arena->alloc_list, 0);
		// We free the block's miniblocks
		block_t *block = block_node->data;
		dll_list_t *minilist = block->miniblock_list;
		while (minilist->size > 0) {
			dll_node_t *miniblock_node = dll_remove_nth_node(minilist, 0);
			// We free the miniblock's data
			miniblock_t *miniblock = miniblock_node->data;
			free(miniblock->rw_buffer);
			free(miniblock_node->data);
			free(miniblock_node);
		}
		free(minilist);
		free(block_node->data);
		free(block_node);
	}
	free(arena->alloc_list);
	free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	// Verify wether or not we can even allocate the block
	if (address >= arena->arena_size) {
		if (address > arena->arena_size)
			printf("The allocated address is outside the size of arena\n");
		else if (size)
			printf("The allocated address is outside the size of arena\n");
		return;
	}
	if (address + size > arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	}
	// We check to see if there is a block already allocated at this address
	dll_node_t *nod = arena->alloc_list->head;
	uint64_t address_end = address + size;
	dll_node_t *merger_node = NULL, *merger_end_node = NULL;
	dll_node_t *first_favor = NULL, *last_favor = NULL;
	block_t *merger = NULL, *merger_end = NULL;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		// We get the extremities
		uint64_t adr, adr_end;
		block_t *bl = (block_t *)nod->data;
		adr = bl->start_address;
		adr_end = adr + bl->size;
		// Check for overlap
		if (!(address_end <= adr || address >= adr_end)) {
			printf("This zone was already allocated.\n");
			return;
		}
		// Check for merging possibilities
		if (address == adr_end) {
			merger_end = bl;
			merger_end_node = nod;
		}
		if (address_end == adr) {
			merger = bl;
			merger_node = nod;
		}
		if (address >= adr_end)
			last_favor = nod;
		if (address_end <= adr && !first_favor)
			first_favor = nod;
		// We continue onto the next block
		nod = nod->next;
	}
	// Block Allocation and merging
	block_t *bl = calloc(1, sizeof(block_t));
	DIE(!bl, "calloc() for block failed");
	bl->start_address = address;
	bl->size = size;
	// Merging to the left side of the block
	if (merger_end) {
		bl->size = bl->size + merger_end->size;
		bl->start_address = merger_end->start_address;
	}
	// Merging to the right side of the block
	if (merger)
		bl->size = bl->size + merger->size;
	// Merging la listele de miniblocuri
	// The left side
	bl->miniblock_list = dll_create(sizeof(miniblock_t));
	dll_list_t *minib_l = bl->miniblock_list, *merged_list;
	int i = 0;
	if (merger_end) {
		merged_list = merger_end->miniblock_list;
		while ((int)merged_list->size > 0) {
			dll_node_t *miniblock_node = dll_remove_nth_node(merged_list, 0);
			dll_add_nth_node(minib_l, i, miniblock_node->data);
			free(miniblock_node->data);
			free(miniblock_node);
			i++;
		}
	}
	// We also add our new miniblock inbetween(address TO address_end)
	miniblock_t *minitmp = calloc(1, sizeof(miniblock_t));
	DIE(!minitmp, "calloc() for miniblock failed");
	minitmp->start_address = address;
	minitmp->size = size;
	minitmp->rw_buffer = calloc(size, sizeof(int8_t));
	DIE(!minitmp->rw_buffer, "calloc() for rw_buffer failed");
	minitmp->perm = 0; // 0 = RW-
	dll_add_nth_node(minib_l, i, minitmp);
	free(minitmp);
	i++;
	//The right side
	int j = 0;
	if (merger) {
		merged_list = merger->miniblock_list;
		while (merged_list->size > 0) {
			dll_node_t *miniblock_node = dll_remove_nth_node(merged_list, 0);
			dll_add_nth_node(minib_l, i + j, miniblock_node->data);
			free(miniblock_node->data);
			free(miniblock_node);
			j++;
		}
	}
	// Node insertion, freeing and removing
	dll_node_t *f_node = calloc(1, sizeof(dll_node_t));
	DIE(!f_node, "calloc() for dll_node failed");
	f_node->data = bl;
	if (merger_end) {
		if (merger_end_node == arena->alloc_list->head)
			arena->alloc_list->head = f_node;
		free(merger_end->miniblock_list);
		free(merger_end_node->data);
		f_node->prev = merger_end_node->prev;
		f_node->next = merger_end_node->next;
		free(merger_end_node);
		arena->alloc_list->size--;
	}
	if (merger) {
		if (merger_node == arena->alloc_list->head)
			arena->alloc_list->head = f_node;
		free(merger->miniblock_list);
		free(merger_node->data);
		if (!f_node->prev)
			f_node->prev = merger_node->prev;
		f_node->next = merger_node->next;
		free(merger_node);
		arena->alloc_list->size--;
	}
	// If this is the first block, we make it point to itself
	if (arena->alloc_list->size == 0) {
		f_node->prev = f_node;
		f_node->next = f_node;
		arena->alloc_list->head = f_node;
		arena->alloc_list->size = 1;
		return;
	}
	// If we didn't merge, we treat separately
	if (!f_node->prev) {
		if (last_favor)
			f_node->prev = last_favor;
		else
			f_node->prev = arena->alloc_list->head->prev;
	}
	if (!f_node->next) {
		if (first_favor) {
			f_node->next = first_favor;
			if (first_favor == arena->alloc_list->head)
				arena->alloc_list->head = f_node;
		} else {
			f_node->next = arena->alloc_list->head;
		}
	}
	// We adjust the next and prev nodes
	f_node->prev->next = f_node;
	f_node->next->prev = f_node;
	arena->alloc_list->size++;
}

void free_block(arena_t *arena, const uint64_t address)
{
	dll_node_t *b_node = arena->alloc_list->head;
	unsigned int i = 0, j = 0;
	for (; i < arena->alloc_list->size; i++, b_node = b_node->next) {
		block_t *block = b_node->data;
		if (address < block->start_address)
			continue;
		if (address > block->start_address + block->size)
			continue;
		dll_list_t *mini_list = block->miniblock_list;
		dll_node_t *m_node = mini_list->head;
		for (; j < mini_list->size; j++, m_node = m_node->next) {
			miniblock_t *minib = m_node->data;
			if (minib->start_address != address)
				continue; // We remove the miniblocks content
			free(minib->rw_buffer);
			int pos = 2; // 0 = beggining, 1 = end, 2 = inside
			if (m_node == mini_list->head)
				pos = 0;
			if (m_node == mini_list->head->prev)
				pos = 1;
			if (pos == 0) // prev, next of m_node, size mini_list
				mini_list->head = m_node->next;
			m_node->prev->next = m_node->next;
			m_node->next->prev = m_node->prev;
			mini_list->size--;
			block->size -= minib->size; // Adjust block size
			if (mini_list->size == 0) {
				if (arena->alloc_list->head == b_node)
					arena->alloc_list->head = b_node->next;
				free(m_node); // We free the miniblock node
				free(minib); // We free the miniblock
				free(mini_list); // We free the miniblock list
				free(b_node->data); // We free the block
				b_node->next->prev = b_node->prev;
				b_node->prev->next = b_node->next;
				free(b_node); // We free the block node
				arena->alloc_list->size--;
				return;
			}
			if (pos == 0) {
				miniblock_t *new_minib = mini_list->head->data;
				block->start_address = new_minib->start_address;
				free(m_node); // We free the miniblock node
				free(minib); // We free the miniblock
				return;
			}
			if (pos == 1) {
				free(m_node); // We free the miniblock node
				free(minib); // We free the miniblock
				return;
			}
			miniblock_t *start_miniblock = mini_list->head->data;
			block_t *new_block = malloc(sizeof(block_t));
			DIE(!new_block, "malloc() for new block failed");
			new_block->start_address = start_miniblock->start_address;
			new_block->size = 0;
			new_block->miniblock_list = malloc(sizeof(dll_list_t));
			DIE(!new_block->miniblock_list, "malloc() failed");
			dll_list_t *newl_mini = new_block->miniblock_list;
			newl_mini->head = NULL;
			newl_mini->size = 0;
			newl_mini->data_size = sizeof(miniblock_t);
			while (mini_list->head != m_node->next) {
				dll_node_t *transfer = dll_remove_nth_node(mini_list, 0);
				dll_add_nth_node(newl_mini, newl_mini->size, transfer->data);
				block->size -= ((miniblock_t *)transfer->data)->size;
				new_block->size += ((miniblock_t *)transfer->data)->size;
				free(transfer->data);
				free(transfer);
			} // We add the new block to the list
			dll_node_t *new_block_node = malloc(sizeof(dll_node_t));
			DIE(!new_block_node, "malloc() for new block node failed");
			new_block_node->data = new_block;
			new_block_node->next = b_node;
			new_block_node->prev = b_node->prev;
			b_node->prev = new_block_node;
			new_block_node->prev->next = new_block_node;
			arena->alloc_list->size++;
			if (i == 0)
				arena->alloc_list->head = new_block_node;
			fix_my_block(block);
			free(m_node); // We free the miniblock node
			free(minib); // We free the miniblock;
			return;
		}
		break;
	}
	printf("Invalid address for free.\n");
}

void read(arena_t *arena, uint64_t address, uint64_t size) {
	// First we have to find the block in which the address is
	dll_list_t *blocks = arena->alloc_list;
	dll_node_t *b_node = blocks->head;
	for (unsigned int i = 0; i < blocks->size; i++, b_node = b_node->next) {
		block_t *block = b_node->data;
		if (address < block->start_address)
			continue;
		if (address > block->start_address + block->size)
			break;
		// If we got here it means we have found the right block
		uint64_t end_address = address + size;
		unsigned int characters_read = (unsigned int)size;
		if(address + size > block->start_address + block->size) {
			end_address = block->start_address + block->size;
			characters_read -= (unsigned int)(address + size - end_address);
			printf("Warning: size was bigger than the block size. ");
			printf("Reading %u characters.\n", characters_read);
		}
		// We remember how many characters we have to read
		uint64_t size_left = 0;
		// We get the miniblocks list and find the first one to read
		dll_list_t *mini_list = block->miniblock_list;
		dll_node_t *m_node = mini_list->head;
		for (unsigned int j = 0; j < mini_list->size; j++, m_node = m_node->next) {
			miniblock_t *minib = m_node->data;
			if (address < minib->start_address)
				if(address + size < minib->start_address + minib->size)
					continue;
			// We have found the first miniblock we can read
			uint64_t diff = address - minib->start_address;
			if(address < minib->start_address)
				diff = 0;
			uint64_t i = diff;
			int8_t *buffer = (int8_t*)minib->rw_buffer;
			for(; size_left < size && i < minib->size; i++, size_left++)
				printf("%c", buffer[i]);
		}
		printf("\n");
		return;
	}
	// If we got here it means there is no block with that address
	printf("Invalid address for read.\n");
}

void write
(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data) {
	// First we have to find the block in which the address is
	dll_list_t *blocks = arena->alloc_list;
	dll_node_t *b_node = blocks->head;
	for (unsigned int i = 0; i < blocks->size; i++, b_node = b_node->next) {
		block_t *block = b_node->data;
		if (address < block->start_address)
			continue;
		if (address > block->start_address + block->size)
			continue;
		// If we got here it means we have found the right block
		uint64_t end_address = address + size;
		unsigned int characters_write = (unsigned int)size;
		if(address + size > block->start_address + block->size) {
			end_address = block->start_address + block->size;
			characters_write -= (unsigned int)(address + size - end_address);
			printf("Warning: size was bigger than the block size. ");
			printf("Writing %u characters.\n", characters_write);
		}
		// We remember how many characters we have to write
		uint64_t size_left = 0;
		// We get the miniblocks list and find the first one to read
		dll_list_t *mini_list = block->miniblock_list;
		dll_node_t *m_node = mini_list->head;
		for (unsigned int j = 0; j < mini_list->size; j++, m_node = m_node->next) {
			miniblock_t *minib = m_node->data;
			if (address < minib->start_address)
				if(address + size < minib->start_address + minib->size)
					continue;
			// We have found the first miniblock we can read
			uint64_t diff = address - minib->start_address;
			if(address < minib->start_address)
				diff = 0;
			uint64_t i = diff;
			int8_t *buffer = (int8_t*)minib->rw_buffer;
			for(; size_left < size && i < minib->size; i++, size_left++)
				buffer[i] = data[size_left];
		}
		return;
	}
	// If we got here it means there is no block with that address
	printf("Invalid address for write.\n");
}

void pmap(const arena_t *arena)
{
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	// Total arena free memory, blocks, miniblocks
	int miniblocks = 0;
	uint64_t left = arena->arena_size;
	dll_node_t *nod = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
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
	printf("Free memory: 0x%lX bytes\n", left);
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %d\n", miniblocks);
	// Blocks statistics
	nod = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *block = nod->data;
		printf("\nBlock %d begin\n", i + 1);
		printf("Zone: 0x%lX - ", block->start_address);
		printf("0x%lX\n", block->start_address + block->size);
		// Miniblocks
		dll_list_t *mini_list = block->miniblock_list;
		dll_node_t *mini_node = mini_list->head;
		for (unsigned int j = 0; j < mini_list->size; j++) {
			miniblock_t *data_mini = mini_node->data;
			printf("Miniblock %d:", j + 1);
			printf("		");
			printf("0x%lX", data_mini->start_address);
			printf("		-		");
			printf("0x%lX		", data_mini->start_address + data_mini->size);
			if (data_mini->perm == 0)
				printf("| RW-\n");
			else
				printf("Nu e gata\n");
			mini_node = mini_node->next;
		}
		printf("Block %d end\n", i + 1);
		nod = nod->next;
	}
}

// Extra
void sim_errorer(int num)
{
	for (int i = 0; i < num; i++)
		printf("Invalid command. Please try again.\n");
}

void errorer(int num, char *tok, const char *delim)
{
	while (tok) {
		printf("Invalid command. Please try again.\n");
		tok = strtok(NULL, delim);
	}
	sim_errorer(num);
}

void fix_my_block(block_t *block) {
	// We want to fix the start and size of this block
	// We get the first miniblock
	dll_list_t *mini_list = block->miniblock_list;
	dll_node_t *mini_node = mini_list->head;
	miniblock_t *mini = mini_node->data;
	block->start_address = mini->start_address;
	// We get the last miniblock
	mini_node = mini_node->prev;
	mini = mini_node->data;
	// We adjust the size of the block
	block->size = (mini->start_address + mini->size) - block->start_address;
}