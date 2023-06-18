#ifndef STACK_H_
#define STACK_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef STACK_TYPE
    #define STACK_TYPE int
#endif

typedef struct {
    STACK_TYPE* items;
    size_t count;
} Stack;

Stack stack_init();
void stack_push(Stack* stack, STACK_TYPE val);
STACK_TYPE stack_pop(Stack* stack);
void stack_deinit(Stack* stack);

#endif // STACK_H_

// Memory util function
static void is_stack_mem_valid(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Failed to get appropriate stack memory size\n");
        assert(false);
    }
}

#ifdef STACK_H_IMPLEMENTATION
Stack stack_init() {
    return (Stack) {
        .items = NULL,
        .count = 0,
    };
}

void stack_push(Stack* stack, STACK_TYPE val) {
    stack->count++;

    stack->items = (STACK_TYPE*)realloc(stack->items, sizeof(STACK_TYPE) * stack->count);
    is_stack_mem_valid(stack->items);
    stack->items[stack->count - 1] = val;
}

STACK_TYPE stack_pop(Stack* stack) {
    stack->count--;

    STACK_TYPE removed_item = stack->items[stack->count];
    stack->items = (STACK_TYPE*)realloc(stack->items, sizeof(STACK_TYPE) * stack->count);
    if (stack->count != 0) {
        is_stack_mem_valid(stack->items);
    }
    return removed_item;
}

void stack_deinit(Stack* stack) {
    free(stack->items);
    stack->count = 0;
}
#endif // STACK_H_IMPLEMENTATION
