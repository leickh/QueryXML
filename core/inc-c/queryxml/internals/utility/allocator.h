
#ifndef QUERYXML_ALLOCATOR_H
#define QUERYXML_ALLOCATOR_H

#include <stddef.h>

typedef struct QxmlAllocator QxmlAllocator;

struct QxmlAllocator
{
    void * (*fn_alloc)(QxmlAllocator *allocator, size_t num_bytes);
    void (*fn_free)(QxmlAllocator *allocator, void *region);
    void * (*fn_realloc)(QxmlAllocator *allocator, void *region, size_t new_num_bytes);
    void (*fn_destroy)(QxmlAllocator *allocator);

    void *user_pointer;
};

QxmlAllocator qxml_libc_allocator();

void * qxml_raw_alloc(
    QxmlAllocator *allocator,
    size_t num_bytes
);

void qxml_raw_free(
    QxmlAllocator *allocator,
    void *region
);

void * qxml_raw_realloc(
    QxmlAllocator *allocator,
    void *region,
    size_t new_region_size
);

void qxml_destroy_allocator(
    QxmlAllocator *allocator
);

QxmlAllocator * qxml_new_dynamic_arena(
    QxmlAllocator *super_allocator,
    size_t initial_footprint,
    size_t maximum_footprint
);

#endif // QUERYXML_ALLOCATOR_H
