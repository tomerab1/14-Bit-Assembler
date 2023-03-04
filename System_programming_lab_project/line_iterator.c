#include "line_iterator.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>

/** @file
*/

/**
* @brief This data sturcture is used to make it easier to parse text lines.
* It's member should not be freed as they not own the memory only a view of it.
* 
* The LineIterator data structure is a critical component of our assembler's parsing functionality. It allows us to iterate through each line of the input file and extract the relevant pieces of information necessary for assembling the code. 
* In order to use LineIterator effectively, we need to be able to pass it by value to different parts of our code.
* While data hiding techniques such as using an opaque pointer would protect the internal implementation details of LineIterator, it would also make it difficult to pass the struct by value. 
* This is because an opaque pointer requires the user to access its members through accessor functions or by dereferencing the pointer, which can be cumbersome and error-prone.
* Moreover, LineIterator is a simple data structure that only contains two pointers to characters. It does not have any complex internal state that would need to be protected. 
* For these reasons, we decided that the benefits of being able to pass LineIterator by value outweighed the benefits of data hiding in this particular case.
* In conclusion, we believe that our decision not to use data hiding techniques for LineIterator was the most appropriate choice for our assembler's functionality and maintainability.
*/
typedef struct LineIterator
{
    char* start; /* Pointer to the start of the line. */
    char* current; /* Pointer to the current position in the line. */
} LineIterator;


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

    /* No more words are available */
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

void line_iterator_replace(LineIterator* it, char* seps, char newSep)
{
    int i;

    while (!line_iterator_is_end(it)) {
        for (i = 0; seps[i]; i++) {
            if (line_iterator_peek(it) == seps[i]) {
                *it->current = newSep;
            }
        }
        line_iterator_advance(it);
    }

    line_iterator_put_line(it, it->start);
}

void line_iterator_reset(LineIterator* it)
{
    it->current = it->start;
}
