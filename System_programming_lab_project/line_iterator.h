#ifndef LINE_ITERATOR_H
#define LINE_ITERATOR_H

/** @file
*/

#include "utils.h"

/**
* @brief This data sturcture is used to make it easier to parse text lines.
* It's member should not be freed as they not own the memory only a view of it.
*
* The LineIterator data structure is a critical component of our assembler's parsing functionality.
* It allows us to iterate through each line of the input file and extract the relevant pieces of information necessary for assembling the code.
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
