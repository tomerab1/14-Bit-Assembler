#include "line_iterator.h"
#include <ctype.h>

void line_iterator_put_line(LineIterator* it, const char* line)
{
    it->current = it->start = line;
}

void line_iterator_advance(LineIterator* it)
{
    if (!line_iterator_is_end(it))
        ++(it->current);
}

void line_iterator_backwards(LineIterator* it)
{
    if (it->current - 1 >= it->start)
        it->current--;
}

void line_iterator_consume_blanks(LineIterator* it)
{
    while (isblank(line_iterator_peek(it)))
        line_iterator_advance(it);
}

char line_iterator_peek(LineIterator* it)
{
    return *(it->current);
}

bool line_iterator_is_end(LineIterator* it)
{
    return *(it->current) == '\0';
}
