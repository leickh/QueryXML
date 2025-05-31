
#ifndef QUERYXML_ARENA_ALLOCATOR_H
#define QUERYXML_ARENA_ALLOCATOR_H

#include <stddef.h>
#include <queryxml/internals/utility/allocator.h>

typedef struct QxmlArenaHead QxmlArenaHead;
typedef struct QxmlArenaBlock QxmlArenaBlock;

struct QxmlArenaHead
{
    size_t maximum_footprint;
    size_t current_footprint;
    QxmlArenaBlock *root_block;
    QxmlAllocator *super_allocator;
};

struct QxmlArenaBlock
{
    size_t capacity;
    size_t usage;
    void *allocation;
    QxmlArenaBlock *continuation;
    QxmlArenaHead *head;
};

#endif // QUERYXML_ARENA_ALLOCATOR_H
