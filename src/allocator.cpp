// allocator.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>

const size_t HEAP_SIZE = 1'000'000'000; // 1GB
uint8_t *heap_memory;

void* tmalloc(size_t size) {
    return nullptr;
}

void tfree(void *ptr) {
}

int main() {
    heap_memory = (uint8_t*)malloc(HEAP_SIZE);

    free(heap_memory);
    return 0;
}
