#ifndef LINE_ITERATOR_H
#define LINE_ITERATOR_H

/** @file
*/

#include "utils.h"

/**
 * This struct is meant for making working with strings while parsing the assembly files.
*/
typedef struct
{
	char* start; /* Pointer to the start of the line. */
	char* current; /* Pointer to the current position in the line. */
} LineIterator;

/**
* This function 'puts' a new line in the iterator.
* @param it - The iterator.
* @param line - The line.
*/
void line_iterator_put_line(LineIterator* it, char* line);

/**
* This function moves the iterator by 1 character, if we reached the end it will do nothing.
* @param it - The iterator.
*/
void line_iterator_advance(LineIterator* it);

/**
* This function moves the iterator backwards by 1 character, if we reached the start it will do nothing.
* @param it - The iterator.
*/
void line_iterator_backwards(LineIterator* it);

/**
* This function ungets an entire word from the iterator, thus restoring the state the iterator was in before we got the word.
* @param it - The iterator.
* @param word - The word to unget.
*/
void line_iterator_unget_word(LineIterator* it, char* word);


/**
* This function consumes blanks, i.e white spaces and tabs.
* @param it - The iterator.
*/
void line_iterator_consume_blanks(LineIterator* it);

/**
* This function moves the iterator past the first occurrence of the specified seperator.
* @param it - The iterator.
* @param sep - The seperator.
*/
void line_iterator_jump_to(LineIterator* it, char sep);

/**
* This function returns the current char.
* @param it - The iterator.
* @return the current char.
*/
char line_iterator_peek(LineIterator* it);

/**
* This function returns the next word in the iterator, it will use 'seps' to determine the end of the word.
* @param it - The iterator.
* @param seps - The seperator.
* @return the current char.
*/
char* line_iterator_next_word(LineIterator* it, char* seps);


/**
* This function checks if the iterator reached its end, i.e '\0'. 
* @param it - The iterator.
* @return True if reached end of line, false otherwise.
*/
bool line_iterator_is_end(LineIterator* it);


/**
* This function checks if the iterator reached line beginning.
* @param it - The iterator.
* @return True if at the beginning of the line, false otherwise.
*/
bool line_iterator_is_start(LineIterator* it);


/**
* This function check if the iterator current char, matches any of the seperators. 
* @param it - The iterator.
* @return True if match, false otherwise.
*/
bool line_iterator_match_any(LineIterator* it, char* seps);

/**
* This function checks if any char exists in line
* @param it - The iterator.
* @param searchFor - The word.
* @return True if word exists, false otherwise.
*/
bool line_iterator_word_includes(LineIterator* it, char* searchFor);

/**
* This function returns the last word in the iterator.
* @param it - The iterator.
* @return The last word.
*/
char* get_last_word(LineIterator* it);

/**
* This function replaces all the occurrences of 'seps' with 'newSep'
* @param it - The iterator.
* @param spes - The seperators to seperate on.
* @param newSep - The new seperator.
*/
void line_iterator_replace(LineIterator* it, char* seps, char newSep);

/**
* This function resets the iterator, i.e moves it back to the start of the line.
* @param it - The iterator.
*/
void line_iterator_reset(LineIterator* it);

#endif
