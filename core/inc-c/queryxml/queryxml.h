
#ifndef QUERYXML_H
#define QUERYXML_H

#include <stddef.h>

typedef void QxmlProcessor;
typedef struct QxmlAllocator QxmlAllocator;
typedef struct QxmlInputStream QxmlInputStream; 

struct QxmlAllocator
{
    void * (*fn_alloc)(QxmlAllocator *allocator, size_t num_bytes);
    void (*fn_free)(QxmlAllocator *allocator, void *region);
    void (*fn_realloc)(QxmlAllocator *allocator, void *region, size_t new_num_bytes);

    void *userpointer;
};

QxmlProcessor * qxml_new_processor(QxmlAllocator allocator);
void qxml_free_processor(QxmlProcessor *processor);

void qxml_parse_streamed(QxmlProcessor *processor, QxmlInputStream *stream);
void qxml_parse_source(QxmlProcessor *processor, const char *source);

QxmlAllocator qxml_libc_allocator();

#endif // QUERYXML_H
