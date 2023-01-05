#ifndef LINE_ITERATOR_H
#define LINE_ITERATOR_H

/** @file
*/

#include "utils.h"

typedef struct
{
	char* start; // Pointer to the start of the line.
	char* current; // Pointer to the current position in the line.
	char* end;
} LineIterator;

/* Puts a new line into the iterator. */
void line_iterator_put_line(LineIterator* it, char* line);

/* Advances the line iterator by 1. */
void line_iterator_advance(LineIterator* it);

/* Advances the line iterator by -1. */
void line_iterator_backwards(LineIterator* it);

/* Advances the line iterator by the words length. */
void line_iterator_unget_word(LineIterator* it, char* word);

/* Consumes blanks, i.e white spaces and tabs. */
void line_iterator_consume_blanks(LineIterator* it);

/* Advances the line iterator until the specified char */
void line_iterator_jump_to(LineIterator* it, char sep);

/* Returns the current char 'current' points to. */
char line_iterator_peek(LineIterator* it);

/* Returns the next word. */
char* line_iterator_next_word(LineIterator* it, char* seps);

/* Checks if the iterator reached its end, i.e '\0'. */
bool line_iterator_is_end(LineIterator* it);

/*Checks if the iterator reached line beginning*/
bool line_iterator_is_start(LineIterator* it);

/* Check if the iterator current char, matche any of the seperators. */
bool line_iterator_match_any(LineIterator* it, char* seps);

/* Checks if any char exists in line*/
bool line_iterator_includes(LineIterator* it, char searchFor);

bool line_iterator_word_includes(LineIterator* it, char* searchFor);

char* get_last_word(LineIterator* it);
#endif
