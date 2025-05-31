
#include <queryxml/internals/utility/arena.h>
#include <queryxml/internals/utility/allocator.h>
#include <queryxml/internals/utility/math.h>

QxmlArenaBlock * qxml_new_arena_block(
    QxmlArenaBlock *super_block
) {
    QxmlArenaHead *head = super_block->head;
    size_t remaining_space = (head->maximum_footprint - head->current_footprint);
    if ( ! head ->maximum_footprint)
    {
        remaining_space = SIZE_MAX;
    }
    size_t len_continuation = qxml_min_sz(
        remaining_space,
        super_block->capacity * 2
    );
    // If there's no more space allowed for this arena
    if (len_continuation < qxml_ceil_pow2_u64(sizeof(QxmlArenaBlock)))
    {
        return NULL;
    }

    void *raw_allocation = qxml_raw_alloc(
        head->super_allocator,
        len_continuation 
    );
    QxmlArenaBlock *block = raw_allocation;
    block->capacity = len_continuation - qxml_ceil_pow2_u64(sizeof(QxmlArenaBlock));
    block->usage = 0;
    block->allocation = (void *)
        ((size_t) raw_allocation
      + qxml_ceil_pow2_u64(sizeof(QxmlArenaBlock))
    );
    block->continuation = NULL;
    block->head = head;
    return block;
}

void qxml_arena_destroy(
    QxmlAllocator *allocator
);
void * qxml_arena_alloc(
    QxmlAllocator *allocator,
    size_t num_bytes
);

QxmlAllocator * qxml_new_dynamic_arena(
    QxmlAllocator *super_allocator,
    size_t initial_footprint,
    size_t maximum_footprint
) {
    if (initial_footprint < 256)
    {
        return NULL;
    }

    void *initial_allocation = qxml_raw_alloc(
        super_allocator,
        initial_footprint
    );
    QxmlArenaHead *arena_head = (void *)
        ((size_t) initial_allocation
      + qxml_ceil_pow2_u64(sizeof(QxmlAllocator))
    );

    arena_head->super_allocator = super_allocator;
    arena_head->current_footprint = initial_footprint;
    arena_head->maximum_footprint = maximum_footprint;

    QxmlArenaBlock *root_block = (void *)
        ((size_t) initial_allocation
      + qxml_ceil_pow2_u64(sizeof(QxmlAllocator))
      + qxml_ceil_pow2_u64(sizeof(QxmlArenaHead))
    );

    void *root_block_allocation = (void *)
        ((size_t) initial_allocation
      + qxml_ceil_pow2_u64(sizeof(QxmlAllocator))
      + qxml_ceil_pow2_u64(sizeof(QxmlArenaHead))
      + qxml_ceil_pow2_u64(sizeof(QxmlArenaBlock))
    );

    uint32_t remaining_memory = initial_footprint
      - qxml_ceil_pow2_u64(sizeof(QxmlAllocator))
      - qxml_ceil_pow2_u64(sizeof(QxmlArenaHead))
      - qxml_ceil_pow2_u64(sizeof(QxmlArenaBlock));

    root_block->capacity = remaining_memory;
    root_block->usage = 0;
    root_block->allocation = root_block_allocation;
    root_block->continuation = NULL;
    root_block->head = arena_head;

    arena_head->root_block = root_block;

    QxmlAllocator *allocator = initial_allocation;
    allocator->fn_alloc = &qxml_arena_alloc;
    allocator->fn_free = NULL;
    allocator->fn_realloc = NULL;
    allocator->fn_destroy = qxml_arena_destroy;
    allocator->user_pointer = arena_head;

    return allocator;
}

void qxml_arena_destroy_block(
    QxmlArenaBlock *block
) {
    if (block->continuation)
    {
        qxml_arena_destroy_block(block->continuation);
    }
    // If this is the root block, its memory belongs to the same
    // allocation as the head structure; it cannot be freed using
    // this function and must be freed with 'qxml_arena_destroy'.
    if (block == block->head->root_block)
    {
        return;
    }
    qxml_raw_free(block->head->super_allocator, block);
}

void qxml_arena_destroy(
    QxmlAllocator *allocator
) {
    QxmlArenaHead *head_structure = allocator->user_pointer;
    qxml_arena_destroy_block(head_structure->root_block);
    qxml_raw_free(
        head_structure->super_allocator,
        allocator
    );
}

void * qxml_arena_alloc_in_block(
    QxmlArenaBlock *block,
    size_t num_bytes
) {
    if ((block->usage + num_bytes) > block->capacity)
    {
        if ( ! block->continuation)
        {
            block->continuation = qxml_new_arena_block(block);

            // If creating a continuation arena failed
            // (if the arena has reached its maximum allowed footprint)
            if ( ! block->continuation)
            {
                return NULL;
            }
        }
        return qxml_arena_alloc_in_block(
            block->continuation,
            num_bytes
        );
    }
    void *allocation = (void *) ((size_t) block->allocation + block->usage); 
    block->usage += num_bytes;

    return allocation;
}

void * qxml_arena_alloc(
    QxmlAllocator *allocator,
    size_t num_bytes
) {
    QxmlArenaHead *head_structure = allocator->user_pointer;
    return qxml_arena_alloc_in_block(
        head_structure->root_block,
        num_bytes
    );
}