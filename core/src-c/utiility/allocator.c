#include <queryxml/internals/utility/allocator.h>
#include <queryxml/internals/utility/math.h>

#include <stdint.h>
#include <stdlib.h>

// Libc-Wrapper Allocator

void * qxml_wrap_alloc_libc(
    QxmlAllocator *allocator,
    size_t num_bytes
) {
    // Avoid warnings from unused argumentskk
    allocator = allocator;

    return malloc(num_bytes);
}

void qxml_wrap_free_libc(
    QxmlAllocator *allocator,
    void *region
) {
    // Avoid warnings from unused arguments
    allocator = allocator;

    if ( ! region)
    {
        return;
    }
    free(region);
}

void * qxml_wrap_realloc_libc(
    QxmlAllocator *allocator,
    void *region,
    size_t new_num_bytes
) {
    // Avoid warnings from unused arguments
    allocator = allocator;

    if ( ! region)
    {
        return NULL;
    }
    return realloc(region, new_num_bytes);
}

QxmlAllocator qxml_libc_allocator()
{
    QxmlAllocator allocator;
    allocator.fn_alloc = &qxml_wrap_alloc_libc;
    allocator.fn_free = &qxml_wrap_free_libc;
    allocator.fn_realloc = &qxml_wrap_realloc_libc;
    return allocator;
}



// Arena Allocator




// Genericized Functions

void * qxml_raw_alloc(
    QxmlAllocator *allocator,
    size_t num_bytes
) {
    // There's no check here for whether fn_alloc is NULL because
    // all allocators have to be able to allocate but not all must
    // be able to free / realloc.

    return allocator->fn_alloc(allocator, num_bytes);
}

void qxml_raw_free(
    QxmlAllocator *allocator,
    void *region
) {
    if ( ! allocator->fn_free)
    {
        return;
    }
    allocator->fn_free(allocator, region);
}

void * qxml_raw_realloc(
    QxmlAllocator *allocator,
    void *region,
    size_t new_region_size
) {
    if ( ! allocator->fn_realloc)
    {
        return NULL;
    }
    return allocator->fn_realloc(allocator, region, new_region_size);
}

void qxml_destroy_allocator(
    QxmlAllocator *allocator
) {
    if ( ! allocator->fn_destroy)
    {
        return;
    }
    allocator->fn_destroy(allocator);
}
