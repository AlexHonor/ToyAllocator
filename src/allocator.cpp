#include <iostream>

const size_t HEAP_SIZE = 1'000; // 1KB
void *heap_memory;

size_t FLAG_BLOCK_ALLOCATED = 0x1;

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
    BlockDescriptor &descriptor = *(BlockDescriptor*)((uint8_t*)ptr - sizeof(BlockDescriptor));
    descriptor.m_flags &= ~FLAG_BLOCK_ALLOCATED;
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

int main() {
    il_init_heap();

    il_print_heap();

    void *block1 = il_alloc(100);
    il_print_heap();
    void *block2 = il_alloc(125);
    il_print_heap();
    il_free(block1);
    il_print_heap();
    block1 = il_alloc(35);
    il_print_heap();
    il_free(block1);
    il_print_heap();
    il_free(block2);
    il_print_heap();

    il_deinit_heap();

    return 0;
}
