
#ifndef QUERYXML_H
#define QUERYXML_H

#include <stddef.h>

#include <queryxml/internals/utility/allocator.h>

typedef struct QxmlProcessor QxmlProcessor;
typedef struct QxmlInputStream QxmlInputStream; 

struct QxmlProcessor
{
    QxmlAllocator *main_allocator;
    QxmlAllocator *permanent_arena;
};

QxmlProcessor * qxml_new_processor(QxmlAllocator allocator);
void qxml_free_processor(QxmlProcessor *processor);

void qxml_parse_streamed(QxmlProcessor *processor, QxmlInputStream *stream);
void qxml_parse_source(QxmlProcessor *processor, const char *source);

QxmlAllocator qxml_libc_allocator();

#endif // QUERYXML_H
