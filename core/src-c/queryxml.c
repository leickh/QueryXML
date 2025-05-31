#include <queryxml/internals/processor.h>

QxmlProcessor * qxml_new_processor(QxmlAllocator allocator)
{
    QxmlAllocator *permanent_arena = qxml_new_dynamic_arena(
        &allocator, // Use the allocator given to this function
        4096,       // Use 4096 initially
        0           // No maximum footprint
    );
    QxmlProcessor *processor = qxml_raw_alloc(
        permanent_arena,
        sizeof(QxmlProcessor)
    );
    processor->permanent_arena = permanent_arena;

    return processor;
}

void qxml_free_processor(QxmlProcessor *processor)
{
    qxml_destroy_allocator(processor->permanent_arena);
}

void qxml_parse_streamed(QxmlProcessor *processor, QxmlInputStream *stream)
{
    //
}

void qxml_parse_source(QxmlProcessor *processor, const char *source)
{
    //
}
