#include "allocator.h"

#include <iostream>

static const size_t HEAP_SIZE = 1'000; // 1KB
static void* heap_memory;

static size_t FLAG_BLOCK_ALLOCATED = 0x1;

struct BlockDescriptor {
    size_t  m_size;
    uint8_t m_flags;
};

void il_init_heap() {
    // @TODO  move to OS virtual memory allocation
    heap_memory = (uint8_t*)malloc(HEAP_SIZE);

    BlockDescriptor &block = *(BlockDescriptor*)heap_memory;
    block.m_size = HEAP_SIZE - sizeof(BlockDescriptor);
    block.m_flags = 0x0;
}

void il_deinit_heap() {
    // @TODO  move to OS virtual memory allocation
    free(heap_memory);
}

// Implicit list implementation
void* il_alloc(size_t size) {
    uint8_t *block_ptr = (uint8_t*)heap_memory;

    while (block_ptr - heap_memory < HEAP_SIZE) {
        BlockDescriptor& block = *(BlockDescriptor*)block_ptr;
        if (block.m_flags & FLAG_BLOCK_ALLOCATED) {
            block_ptr += block.m_size + sizeof(BlockDescriptor);
            continue;
        }
        if (block.m_size >= size) {
            // If we have place left for a new block (Descriptor + some leftover space) 
            if (block.m_size > size + sizeof(BlockDescriptor)) {
                block.m_flags |= FLAG_BLOCK_ALLOCATED;
                size_t new_block_size = block.m_size - size - sizeof(BlockDescriptor);
                block.m_size = size;
                void *ret_ptr = block_ptr + sizeof(BlockDescriptor);
                block_ptr += block.m_size + sizeof(BlockDescriptor);

                // Construct new block
                BlockDescriptor& new_block = *(BlockDescriptor*)block_ptr;
                new_block.m_flags = 0x0;
                new_block.m_size  = new_block_size;

                return ret_ptr;
            } else {
                block.m_flags |= FLAG_BLOCK_ALLOCATED;
                void* ret_ptr = block_ptr + sizeof(BlockDescriptor);
                
                return ret_ptr;
            }
        } else {
            block_ptr += block.m_size + sizeof(BlockDescriptor);
            continue;
        }
    }

    // Failed to allocate
    return nullptr;
}

void il_free(void *ptr) {
    uint8_t* block_ptr = (uint8_t*)heap_memory;
    uint8_t* merge_block_ptr = (uint8_t*)heap_memory;
    bool is_previous_free = true;

    ((BlockDescriptor*)((uint8_t*)ptr - sizeof(BlockDescriptor)))->m_flags = 0;

    while (block_ptr != (uint8_t*)ptr - sizeof(BlockDescriptor) && block_ptr - heap_memory < HEAP_SIZE) {
        BlockDescriptor& block = *(BlockDescriptor*)block_ptr;

        if (!is_previous_free && !(block.m_flags & FLAG_BLOCK_ALLOCATED)) {
            merge_block_ptr = block_ptr;
        }

        is_previous_free = !(block.m_flags & FLAG_BLOCK_ALLOCATED);
        block_ptr += block.m_size + sizeof(BlockDescriptor);
    }

    size_t total_size = 0;
    block_ptr = merge_block_ptr;

    while (block_ptr - heap_memory < HEAP_SIZE) {
        BlockDescriptor& block = *(BlockDescriptor*)block_ptr;
        if (block.m_flags & FLAG_BLOCK_ALLOCATED) {
            break;
        }
        total_size += block.m_size + sizeof(BlockDescriptor);
        block_ptr += block.m_size + sizeof(BlockDescriptor);
    }

    BlockDescriptor& block = *(BlockDescriptor*)merge_block_ptr;
    block.m_size = total_size - sizeof(BlockDescriptor);
    //BlockDescriptor& descriptor = *(BlockDescriptor*)((uint8_t*)ptr - sizeof(BlockDescriptor));
    //descriptor.m_flags &= ~FLAG_BLOCK_ALLOCATED;
}

void il_print_heap() {
    uint8_t* block_ptr = (uint8_t*)heap_memory;

    printf("<-------- Heap Dump --------->\n");

    while (block_ptr - heap_memory < HEAP_SIZE) {
        BlockDescriptor& block = *(BlockDescriptor*)block_ptr;

        if (block.m_flags & FLAG_BLOCK_ALLOCATED) {
            printf("alloced ");
        } else {
            printf("free    ");
        }
        printf("ptr: %x size: %u\n", block_ptr, block.m_size);

        block_ptr += block.m_size + sizeof(BlockDescriptor);
    }

    printf("<------- End Heap Dump ------->\n");
}
