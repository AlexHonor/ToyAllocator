#pragma once

void il_init_heap();
void il_deinit_heap();

// Implicit list implementation
void* il_alloc(size_t size);
void  il_free(void *ptr);
void  il_print_heap();
