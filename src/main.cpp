#include "allocator.h"

int main(int argc, const char **argv) {
    il_init_heap();

    il_print_heap();

    void* block1 = il_alloc(100);
    il_print_heap();
    void* block2 = il_alloc(125);
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
