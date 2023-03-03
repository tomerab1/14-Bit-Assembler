#include "line_iterator.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>

/** @file
*/

/**
* Puts @a line into the line iterator. This is equivalent to iterating over the line and putting each character in @a line into the line iterator.
* 
* @param it
* @param line
*/
void line_iterator_put_line(LineIterator* it, char* line)
{
    it->current = it->start = line;
}

/**
* Advance \ a it to the next line. This is equivalent to \ a next - > current but doesn't affect the position of \ a it.
* 
* @param it
*/
void line_iterator_advance(LineIterator* it)
{
    if (!line_iterator_is_end(it))
        ++(it->current);
}

/**
* Move the iterator backwards. This is equivalent to moving the iterator to the previous character in the line.
* 
* @param it
*/
void line_iterator_backwards(LineIterator* it)
{
    if (it->current - 1 >= it->start)
        it->current--;
}

/**
* Move the iterator to the end of the word. This is equivalent to line_iterator_backwards ( it ) but doesn't change the position of the iterator.
* 
* @param it
* @param word
*/
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

/**
* Consume all blank lines from @a it. This is equivalent to the consume function of the C library.
* 
* @param it
*/
void line_iterator_consume_blanks(LineIterator* it)
{
    while (isblank(line_iterator_peek(it)))
        line_iterator_advance(it);
}

/**
* Jump to the next occurence of sep. This is useful for iterating over a file or when you want to know where to start iterating in the middle of a file.
* 
* @param it
* @param sep - Separator to look for
*/
void line_iterator_jump_to(LineIterator* it, char sep)
{
    char* loc = strchr(it->current, sep);
    if (loc == NULL)
        return;
    it->current = loc + 1;
}

/**
* Peek at the next character without advancing the iterator. This is useful for checking if there is a character to be returned or not.
* 
* @param it
* 
* @return Char that was peeked or \ c 0 if no character was found at the end of the line
*/
char line_iterator_peek(LineIterator* it)
{
    return *(it->current);
}

/**
* Get the next word from the line. This is used to parse words that are part of a word separated by white space.
* 
* @param it
* @param seps - List of characters separating words. May be NULL in which case whitespace is ignored.
* 
* @return Pointer to the word or NULL if there are no more words to get. The caller is responsible for freeing it
*/
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

/**
* Match the next characters in the line. This is useful for checking if there are more than one character to match at the same time.
* 
* @param it
* @param seps - A pointer to the string to match.
* 
* @return TRUE if the iterator is positioned at the first character in the string that matches or FALSE otherwise. If TRUE is returned the iterator is positioned at the character following the match
*/
bool line_iterator_match_any(LineIterator* it, char* seps)
{
    while (*seps) {
        if (line_iterator_peek(it) == *seps)
            return TRUE;
        seps++;
    }

    return line_iterator_peek(it) == *seps;
}

/**
* Returns true if @a it is at the end of the line. This is useful for determining if a line has been skipped or not.
* 
* @param it
* 
* @return @c true if @a it is at the end of the line @c false otherwise. The iterator is positioned at the end
*/
bool line_iterator_is_end(LineIterator* it)
{
    return *(it->current) == '\0';
}


/**
* Check if the iterator is at the start of the line. This is useful for iterating over a line without having to worry about the position of the iterator in the source file.
* 
* @param it
* 
* @return True if \ a it is at the start of the line false otherwise. \ sa line_iterator_is_end
*/
bool line_iterator_is_start(LineIterator* it)
{
    return (it->current) - (it->start) == 0;
}

/**
* Check if the current line contains a word. This is useful for finding the start of a line that is being searched for.
* 
* @param it
* @param searchFor
* 
* @return true if @a searchFor is found false otherwise. The iterator is advanced past the search term if it is found
*/
bool line_iterator_word_includes(LineIterator* it, char* searchFor)
{
    return strstr(it->current, searchFor);
}

/**
* Get the last word in the line. This is useful for looking up the name of a program or command that is being run.
* 
* @param it
* 
* @return Pointer to the word or NULL if there is
*/
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
