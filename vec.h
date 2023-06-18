#ifndef VEC_H_
#define VEC_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef VEC_TYPE
    #define VEC_TYPE int
#endif

typedef struct {
	VEC_TYPE* items;
	size_t length;
} Vec;

Vec vec_init();
void vec_append(Vec* vec, VEC_TYPE val);
void vec_insert(Vec* vec, VEC_TYPE val, size_t index);
VEC_TYPE vec_remove(Vec* vec, size_t index);
void vec_print(const Vec* const vec);
void vec_deinit(Vec* vec);

#endif // VEC_H_

// Memory util function
static void is_vec_mem_valid(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Failed to get appropriate vec memory size\n");
        assert(false);
    }
}

#ifdef VEC_H_IMPLEMENTATION
Vec vec_init() {
	return (Vec) {
		.items = NULL,
		.length = 0,
	};
}

void vec_append(Vec* vec, VEC_TYPE val) {
	// Increase the length attribute to account for the new value
	vec->length++;
	// Reallocate the array to fit the new value
	vec->items = (VEC_TYPE*)realloc(vec->items, sizeof(VEC_TYPE)*vec->length);
    is_vec_mem_valid(vec->items);
	// Place the final value at the end of the array, i.e. append it
	vec->items[vec->length - 1] = val;
}

void vec_insert(Vec* vec, VEC_TYPE val, size_t index) {
	assert(index <= vec->length);
	// Increase the length attribute to account for the new value
	if (index == vec->length) {
		vec_append(vec, val);
		return;
	}
	vec->length++;
	// Reallocate the array to fit the new value
	vec->items = (VEC_TYPE*)realloc(vec->items, sizeof(VEC_TYPE)*vec->length);
    is_vec_mem_valid(vec->items);
	// Traverse the array backwards and move every value with an index
	// greater than the specified index one position to the right
	for (size_t i = vec->length - 1; i > index; i--) {
		vec->items[i] = vec->items[i - 1];
	}
	// At the specified index, place the specified value
	vec->items[index] = val;
}

VEC_TYPE vec_remove(Vec* vec, size_t index) {
    assert(index < vec->length);
    // Decrement the length of the vector
    vec->length--;
    VEC_TYPE removed_value = vec->items[index];

    // Grab the value to the right starting from current
    // and shift it right
    for (size_t i = index; i < vec->length; i++) {
        vec->items[i] = vec->items[i+1];
    }
	// Reallocate the array to removed the value at `index`
	vec->items = (VEC_TYPE*)realloc(vec->items, sizeof(VEC_TYPE)*vec->length);
    is_vec_mem_valid(vec->items);

    return removed_value;
}

void vec_deinit(Vec* vec) {
	free(vec->items);
    vec->length = 0;
}

#endif // VEC_H_IMPLEMENTATION
