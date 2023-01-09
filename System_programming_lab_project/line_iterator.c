#include "line_iterator.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>

/** @file
*/

void line_iterator_put_line(LineIterator* it, char* line)
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

void line_iterator_unget_word(LineIterator* it, char* word)
{
    size_t length;
    if (!word) return;

    length = strlen(word);
    while (length > 0) {
        line_iterator_backwards(it);
        length--;
    }
}

void line_iterator_consume_blanks(LineIterator* it)
{
    while (isblank(line_iterator_peek(it)))
        line_iterator_advance(it);
}

void line_iterator_jump_to(LineIterator* it, char sep)
{
    char* loc = strchr(it->current, sep);
    if (loc == NULL)
        return;
    it->current = loc + 1;
}

char line_iterator_peek(LineIterator* it)
{
    return *(it->current);
}

char* line_iterator_next_word(LineIterator* it, char* seps)
{
    int log_sz = 0, phy_sz = 4;
    char* word = (char*)xcalloc(phy_sz, sizeof(char));

    /* Consume all white spaces */
    line_iterator_consume_blanks(it);

    while (!line_iterator_is_end(it) && !line_iterator_match_any(it, seps)) {
        if (log_sz + 1 >= phy_sz) {
            GROW_CAPACITY(phy_sz);
            word = GROW_ARRAY(char*, word, phy_sz, sizeof(char));
        }
        word[log_sz++] = line_iterator_peek(it);
        line_iterator_advance(it);
    }

    // No more words are available
    if (log_sz == 0) {
        free(word);
        return NULL;
    }

    if (log_sz + 1 < phy_sz) {
       word = GROW_ARRAY(char*, word, log_sz + 1, sizeof(char));
    }

    word[log_sz] = '\0';
    return word;
}

bool line_iterator_match_any(LineIterator* it, char* seps)
{
    while (*seps) {
        if (line_iterator_peek(it) == *seps)
            return TRUE;
        seps++;
    }

    return line_iterator_peek(it) == *seps;
}

bool line_iterator_is_end(LineIterator* it)
{
    return *(it->current) == '\0';
}


bool line_iterator_is_start(LineIterator* it)
{
    return (it->current) - (it->start) == 0;
}

bool line_iterator_word_includes(LineIterator* it, char* searchFor)
{
    return strstr(it->current, searchFor);
}

char* get_last_word(LineIterator* it) {
    char* tempItLocation = it->current;
    line_iterator_jump_to(it, '\0');
    line_iterator_backwards(it);
    it->current = tempItLocation;
    return line_iterator_next_word(it, " ");
}