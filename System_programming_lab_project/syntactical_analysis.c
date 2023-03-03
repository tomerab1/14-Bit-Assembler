/** @file
*/

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include <string.h>
#include <ctype.h>

/**
* Check syntax of label. This is a helper function for get_label_from_string and get_label_from_string.
* 
* @param label
* 
* @return ERROR_CODE_INVALID_LABEL_DEF if label is syntactically
*/
errorCodes check_label_syntax(char* label)
{
    char* colon_loc = strrchr(label, COLON_CHAR);

    if (colon_loc == NULL)
        return ERROR_CODE_SYNTAX_ERROR;

    /* Must end with a colon. */
    if (*(colon_loc + 1) != '\0')
        return ERROR_CODE_INVALID_LABEL_DEF;

    if (!isalpha(*label))
        return ERROR_CODE_INVALID_LABEL_DEF;

    if (isspace(*(colon_loc - 1)))
        return ERROR_CODE_SPACE_BEFORE_COLON;

    if ((colon_loc - label) == 2 && *label == REG_BEG_CHAR) {
    }

    while (label < colon_loc) {
        if (!isalpha(*label) && !isdigit(*label))
            return ERROR_CODE_INVALID_CHAR_IN_LABEL;
        label++;
    }

    trim_symbol_name(label);
    if (cmp_register_name(label))
        return ERROR_CODE_RESERVED_KEYWORD_DEF;

    return ERROR_CODE_OK;
}

/**
* Checks if a label is valid. This is a wrapper for check_label_syntax that returns TRUE if the label is valid and FALSE otherwise.
* 
* @param label
* 
* @return TRUE if the label is valid FALSE otherwise. Note that this does not check if the label is valid
*/
bool is_valid_label(char* label)
{
    return (check_label_syntax(label) == ERROR_CODE_OK) ? TRUE : FALSE;
}

/**
* Check if the next word is a label. This is used to detect whether or not we are in a label context.
* 
* @param line
* 
* @return @c true if the next word is a label @c false otherwise
*/
bool isLabel(LineIterator* line) {
    return is_valid_label(line_iterator_next_word(line, " "));
}

/**
* Verify the syntax of a command. This is a helper function for debug_command ().
* 
* @param it - LineIterator to use for verification. It is assumed that the parser has already advanced through the line that is being processed.
* @param dbg_list - The debugList to which the command belongs.
* 
* @return TRUE if the syntax is correct FALSE otherwise. In this case the parser should be instructed to abort
*/
bool verify_command_syntax(LineIterator* it, debugList* dbg_list)
{
    return FALSE;
}

/**
* Trim the symbol name to make it easier to read. This is used to prevent accidental redefinitions of symbols that are in the middle of a function name.
* 
* @param sym
*/
void trim_symbol_name(char* sym)
{
    /* The ':' must be coupled with the name*/
    sym[strlen(sym) - 1] = '\0';
}

/**
* Compare register name to known ones. This is used to determine if we are interested in a register
* 
* @param str
* 
* @return TRUE if name is known else FALSE ( case insensitive
*/
bool cmp_register_name(char* str)
{
    
    if (strcmp(str, "r0") == 0) return TRUE;
    if (strcmp(str, "r1") == 0) return TRUE;
    if (strcmp(str, "r2") == 0) return TRUE;
    if (strcmp(str, "r3") == 0) return TRUE;
    if (strcmp(str, "r4") == 0) return TRUE;
    if (strcmp(str, "r5") == 0) return TRUE;
    if (strcmp(str, "r6") == 0) return TRUE;
    if (strcmp(str, "r7") == 0) return TRUE;
    return FALSE;
}

/**
* Get opcode from string. This is used to parse opcodes that are passed to libc functions.
* 
* @param str
* 
* @return OpCode corresponding to str or 0 if str doesn't match any opcode
*/
Opcodes get_opcode(char* str)
{
    if (!str)
        return OP_UNKNOWN;

    if (strcmp(str, "mov") == 0) return OP_MOV;
    if (strcmp(str, "cmp") == 0) return OP_CMP;
    if (strcmp(str, "add") == 0) return OP_ADD;
    if (strcmp(str, "sub") == 0) return OP_SUB;
    if (strcmp(str, "not") == 0) return OP_NOT;
    if (strcmp(str, "clr") == 0) return OP_CLR;
    if (strcmp(str, "lea") == 0) return OP_LEA;
    if (strcmp(str, "inc") == 0) return OP_INC;
    if (strcmp(str, "dec") == 0) return OP_DEC;
    if (strcmp(str, "jmp") == 0) return OP_JMP;
    if (strcmp(str, "bne") == 0) return OP_BNE;
    if (strcmp(str, "red") == 0) return OP_RED;
    if (strcmp(str, "prn") == 0) return OP_PRN;
    if (strcmp(str, "jsr") == 0) return OP_JSR;
    if (strcmp(str, "rts") == 0) return OP_RTS;
    if (strcmp(str, "stop") == 0) return OP_STOP;

    return OP_UNKNOWN;
}

/**
* Validate the syntax at the current position. This is a wrapper around validate_syntax_opcode () validate_syntax_data () and validate_syntax_string () to allow custom code to be added to the debugList
* 
* @param it - Iterator over the line to be validated
* @param state - The first pass state of the syntax
* @param line - The line number at which the syntax is being validated
* @param dbg_list - The list of debuggers to which warnings / errors are added
* 
* @return TRUE if the syntax is valid FALSE if it is
*/
bool validate_syntax(LineIterator it, firstPassStates state, long line, debugList* dbg_list)
{
    switch (state) {
    case FP_SYM_DEF:
    case FP_OPCODE: return validate_syntax_opcode(&it, line, dbg_list);
    case FP_SYM_DATA: return validate_syntax_data(&it, line, dbg_list);
    case FP_SYM_STR: return validate_syntax_string(&it, line, dbg_list);
    case FP_SYM_ENT:
    case FP_SYM_EXT: return validate_syntax_extern_and_entry(&it, line, dbg_list);
    }

    return TRUE;
}

/**
* Validates a syntax string. This is a helper function for debug_list_validate_syntax ().
* 
* @param it
* @param line - Line number of the string being validated
* @param dbg_list - Debug list to add nodes to
* 
* @return TRUE if the string is valid FALSE if it is
*/
bool validate_syntax_string(LineIterator* it, long line, debugList* dbg_list)
{
    /* Consume all preceding blanks */
    line_iterator_consume_blanks(it);

    /* Next char of a valid string must be '\"' */
    if (line_iterator_peek(it) != QUOTE_CHAR) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_MISSING_OPEN_QUOTES));
        return FALSE;
    }

    /* Consume quotes*/
    line_iterator_advance(it);

    while (!line_iterator_is_end(it) && line_iterator_peek(it) != QUOTE_CHAR)
        line_iterator_advance(it); 

    /* Check closing quotes */
    if (line_iterator_peek(it) != QUOTE_CHAR) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_MISSING_CLOSE_QUOTES));
        return FALSE;
    }

    /* Consume quotes*/
    line_iterator_advance(it);

    /* Consume all blanks */
    line_iterator_consume_blanks(it);

    if (!line_iterator_is_end(it)) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
        return FALSE;
    }

    return TRUE;
}

/**
* Extract the sentence type from the line. This is used to determine whether or not we are dealing with a sentence that does not start with a newline or is part of an instruction.
* 
* @param it
* 
* @return SENTENCE_TYPE or EMPTY_SENTENCE if there is no sentence
*/
int extract_sentence_type(LineIterator* it) {
    it->current = it->start;
    /*empty sentence*/
    if (line_iterator_is_end(it)) {
        it->current = it->start;
        return EMPTY_SENTENCE;
    }
    if (directive_exists_basic(it)) return DIRECTIVE_SENTENCE; /*directive sentence*/
}

/**
* Checks if a directive exists. This is a basic check that doesn't look for ". " and it's the only way to check for it.
* 
* @param line
* 
* @return TRUE if there is a directive FALSE otherwise. The iterator is advanced past the directive
*/
bool directive_exists_basic(LineIterator* line) {
    while (!line_iterator_is_end(line)) {
        if (line_iterator_peek(line) == DOT_COMMAND) {
            return TRUE;
        }
        line_iterator_advance(line);
    }
    return FALSE;
    line->current = line->start;
}

/**
* Check if instruction at line is a label. This is used to detect if we are going to write an instruction to a local variable that is not 16 - bit.
* 
* @param line
* 
* @return TRUE if instruction exists FALSE
*/
bool find_if_instruction_exists(LineIterator* line) {
    if (isLabel(line)) {
        skip_label_basic(line);
        int localOpcode= get_opcode(line_iterator_next_word(line, " "));

        if (localOpcode != 16)
            return TRUE;
    }
    return FALSE;
}

/**
* Skip a basic label. This is used to skip labels that start with a colon or are followed by blanks.
* 
* @param line
*/
void skip_label_basic(LineIterator* line) {
    line->current = line->start;
    while (!line_iterator_is_end(line) && line_iterator_peek(line) != COLON) {
        line_iterator_advance(line);
    }
    line_iterator_advance(line);
    line_iterator_consume_blanks(line);
}

bool validate_syntax_data(LineIterator* it, long line, debugList* dbg_list)
{
    bool is_valid = TRUE;

    /* Consume blanks */
    line_iterator_consume_blanks(it);
    
    /* Routine to check digit list */
    while (!line_iterator_is_end(it) && is_valid) {
        line_iterator_consume_blanks(it);
        is_valid = verify_int(it, line, ", ", dbg_list);
        line_iterator_advance(it);
    }

    if (!is_valid) {
        return FALSE;
    }

    /* Go backwards */
    while (isspace(line_iterator_peek(it)) || line_iterator_peek(it) == '\0') {
        line_iterator_backwards(it);
    }

    /* Check last char.*/
    if (!isdigit(line_iterator_peek(it))) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
        return FALSE;
    }

    return TRUE;
}

/**
* Validates the syntax extern and entry. This is used to validate the syntax extern and entry in a macro expansion.
* 
* @param it
* @param line - Line number of the comment
* @param dbg_list
* 
* @return TRUE if there is at least one error FALSE if
*/
bool validate_syntax_extern_and_entry(LineIterator* it, long line, debugList* dbg_list)
{
    line_iterator_consume_blanks(it);

    if (!line_iterator_is_end(it)) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
        return FALSE;
    }

    return TRUE;
}

/**
* Validate a syntax opcode. This is a helper function for validate_opcode ().
* 
* @param it
* @param line - Line number on which the parser was started.
* @param dbg_list - Debug list to add warnings / errors to.
* 
* @return true if the opcode is valid false otherwise. Note that it is up to the caller to free it
*/
bool validate_syntax_opcode(LineIterator* it, long line, debugList* dbg_list)
{
    char* word = NULL;

    while ((word = line_iterator_next_word(it, " ")) != NULL) {
        /* check_for_invalid_comma(word); */
        switch (get_syntax_group(word)) {
        case SG_GROUP_1: 
            free(word);
            return match_syntax_group_1(it, line, dbg_list);
        case SG_GROUP_2: 
            free(word);
            return match_syntax_group_2(it, line, dbg_list);
        case SG_GROUP_3: 
            free(word);
            return match_syntax_group_3(it, line, dbg_list);
        case SG_GROUP_4: 
            free(word);
            return match_syntax_group_4(it, line, dbg_list);
        case SG_GROUP_5: 
            free(word);
            return match_syntax_group_5(it, line, dbg_list);
        case SG_GROUP_6:
            free(word);
            return match_syntax_group_6(it, line, dbg_list);
        case SG_GROUP_7:
            free(word);
            return match_syntax_group_7(it, line, dbg_list);
        case SG_GROUP_INVALID:
            free(word);
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYNTAX_ERROR));
            return FALSE;
        }
        /* Check syntax for .data/.string, otherwise it's an error. */
        free(word);
    }

    return TRUE;
}

/**
* Matches a syntax group 1. This is the first part of the syntax group : it must be at the start of a variable or variable declaration.
* 
* @param it
* @param line - The current line number being matched. Used for error reporting.
* @param dbg_list - The debug list to add debug nodes to.
* 
* @return TRUE if a match was found FALSE otherwise. The match is terminated by a newline
*/
bool match_syntax_group_1(LineIterator* it, long line, debugList* dbg_list)
{
    /************************* Match operand 1 ****************************/
    line_iterator_consume_blanks(it);

    /* Check if immediate number */
    if (line_iterator_peek(it) == HASH_CHAR) {
        if (!match_operand(it, line, FLAG_NUMBER, dbg_list))
            return FALSE;
    }
    /* Check if register */
    else if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    line_iterator_consume_blanks(it);

    /************************* Match operand 2 ****************************/

    /* Check if immediate number */
    if (line_iterator_peek(it) == HASH_CHAR) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_SYNTAX_ERROR));
        return FALSE;
    }
    /* Check if register */
    else if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}

/**
* Matches a syntax group 2. This is the second part of the syntax group matching algorithm.
* 
* @param it
* @param line - Line number of the source code. This is used for reporting errors.
* @param dbg_list - Debug list to fill with debug nodes.
* 
* @return TRUE on success FALSE on failure. If FALSE is returned the iterator is not advanced
*/
bool match_syntax_group_2(LineIterator* it, long line, debugList* dbg_list)
{
    /************************* Match operand 1 ****************************/
    line_iterator_consume_blanks(it);

    /* Check if immediate number */
    if (line_iterator_peek(it) == HASH_CHAR) {
        if (!match_operand(it, line, FLAG_NUMBER, dbg_list))
            return FALSE;
    }
    /* Check if register */
    else if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    line_iterator_consume_blanks(it);

    /************************* Match operand 2 ****************************/

    /* Check if immediate number */
    if (line_iterator_peek(it) == HASH_CHAR) {
        if (!match_operand(it, line, FLAG_NUMBER, dbg_list))
            return FALSE;
    }
    /* Check if register */
    else if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}

/**
* Matches a syntax group 3. This is the third part of the syntax group matching algorithm.
* 
* @param it
* @param line - Line number being matched. Used for error reporting.
* @param dbg_list - Debug list for the matched nodes.
* 
* @return TRUE if a match was found FALSE otherwise. When false the iterator is left pointing at the character immediately following the end of the syntax group
*/
bool match_syntax_group_3(LineIterator* it, long line, debugList* dbg_list)
{
    line_iterator_consume_blanks(it);

    /* Check if register */
    if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}

/**
* Matches a syntax group 4. This is used to check if there is a'%'followed by an operand.
* 
* @param it
* @param line - Line number where the token begins. This is used for reporting errors.
* @param dbg_list - Debug list to add matched nodes to.
* 
* @return TRUE if a match was found FALSE otherwise. If FALSE is returned the iterator is not advanced
*/
bool match_syntax_group_4(LineIterator* it, long line, debugList* dbg_list)
{
    /* Check that they dont get any operand. */
    char* op = line_iterator_next_word(it, " ");

    if (op != NULL) {
        free(op);
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    free(op);
    return TRUE;
}

/**
* Matches a syntax group 5 starting at the current position. This is used to implement the syntax group 5 ( and below ) as well as the syntax group 5 ( and below ).
* 
* @param it
* @param line - Line number of the current position
* @param dbg_list - Debug list for the matched nodes
* 
* @return TRUE if a match was found FALSE if not ( or an error occurred
*/
bool match_syntax_group_5(LineIterator* it, long line, debugList* dbg_list)
{
    char* open_paren_loc = strchr(it->current, OPEN_PAREN_CHAR);

    line_iterator_consume_blanks(it);

    /* Check if label name */
    if (!open_paren_loc && isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else if (open_paren_loc) {
        if (line_iterator_peek(it) == REG_BEG_CHAR) {
            if (is_register_name(it)) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_RESERVED_KEYWORD_DEF));
                return FALSE;
            }
            line_iterator_backwards(it);
        }
        if (!is_label_name(it)) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_LABEL_DEF));
            return FALSE;
        }
        
        line_iterator_jump_to(it, '(');

        if (!match_operand(it, line, FLAG_PARAM_LABEL, dbg_list)) {
            return FALSE;
        }
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}

/**
* Matches a syntax group 6 starting at the current position. This is used for variable and variable references.
* 
* @param it
* @param line - Line in the source file being scanned
* @param dbg_list - Debug list for the matches.
* 
* @return TRUE if a match was found FALSE otherwise ( an error has been reported
*/
bool match_syntax_group_6(LineIterator* it, long line, debugList* dbg_list)
{
    line_iterator_consume_blanks(it);

    /* Check if immediate number */
    if (line_iterator_peek(it) == HASH_CHAR) {
        if (!match_operand(it, line, FLAG_NUMBER, dbg_list))
            return FALSE;
    }
    /* Check if register */
    else if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}

/**
* Matches a syntax group 7. In this case we are interested in the name of a register or a label.
* 
* @param it
* @param line - Line number where the syntax group starts. Used for error reporting.
* @param dbg_list - Debug list for all matches.
* 
* @return TRUE if a match was found FALSE otherwise. This function is called by match_syntax_group ()
*/
bool match_syntax_group_7(LineIterator* it, long line, debugList* dbg_list)
{
    line_iterator_consume_blanks(it);

    /************************* Match operand 1 ****************************/

    if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    line_iterator_consume_blanks(it);

    /************************* Match operand 2 ****************************/

    /* Check if register */
    if (line_iterator_peek(it) == REG_BEG_CHAR) {
        if (!match_operand(it, line, FLAG_REGISTER, dbg_list))
            return FALSE;
    }
    /* Check if label name */
    else if (isalpha(line_iterator_peek(it))) {
        if (!match_operand(it, line, FLAG_LABEL, dbg_list))
            return FALSE;
    }
    else {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
        return FALSE;
    }

    return TRUE;
}


/**
* Matches an operand and returns TRUE if it matches. This is a low - level function used by match_operand_p () and match_operand_p2 ().
* 
* @param it
* @param line - Line number of the operand. This is used for error reporting.
* @param flags - Flags to match the operand with. Flags can be FLAG_LABEL FLAG_NUMBER FLAG_REAL_REG and FLAG_REG_NEAREST
* @param dbg_list - Debug list to fill with nodes.
* 
* @return TRUE if the operand matches FALSE otherwise. On failure the iterator is left pointing at the end of the operand
*/
bool match_operand(LineIterator* it, long line, int flags, debugList* dbg_list)
{
    switch (flags) {
    case FLAG_LABEL: 
        if (!is_label_name(it)) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_NAME));
            return FALSE;
        }
        if (!validate_label_ending(it)) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
            return FALSE;
        }

        break;
    case FLAG_NUMBER: 
        /* Consume the '#' */
        line_iterator_advance(it);
        if (!verify_int(it, line, ",", dbg_list)) {
            return FALSE;
        }
        break;
    case FLAG_REGISTER:
        if (is_register_name_heuristic(*it)) {
            if (!is_register_name(it)) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_NAME));
                return FALSE;
            }
            /* Consume the number. */
            line_iterator_advance(it);
        }
        break;
    case FLAG_PARAM_LABEL:
        if (!match_pamaetrized_label(it, line, dbg_list)) {
            return FALSE;
        }
        break;
    default:
        break;
    }

    /* Comsume comma */
    line_iterator_jump_to(it, COMMA_CHAR);

    return TRUE;
}

/**
* Matches pamaetrized labels. This is a bit tricky because we don't know the number of arguments to a label.
* 
* @param it
* @param line - Line number where the pattern was found. Used for error reporting.
* @param dbg_list - Debug list to add warnings to.
* 
* @return TRUE if it succeeds FALSE otherwise. Note that it does not check for syntax errors
*/
bool match_pamaetrized_label(LineIterator* it, long line, debugList* dbg_list)
{
    int comma_counter = 0, close_paren_counter = 0, args_count = 0;

    while (!line_iterator_is_end(it) && line_iterator_peek(it) != CLOSE_PAREN_CHAR) {
        if (line_iterator_peek(it) == HASH_CHAR) {
            line_iterator_advance(it);
            if (!verify_int(it, line, ",)", dbg_list)) {
                return FALSE;
            }
            args_count++;
        }
        else if (isalpha(line_iterator_peek(it))) {
            if (line_iterator_peek(it) == REG_BEG_CHAR) {
                if (!is_register_name(it)) {
                    return FALSE;
                }
                /* Consume the digit. */
                line_iterator_advance(it);
                args_count++;
            }
            else {
                if (!is_label_name(it)) {
                    return FALSE;
                }
                args_count++;
            }
        }
        else if (line_iterator_peek(it) == COMMA_CHAR && comma_counter < 1) {
            line_iterator_advance(it);
            comma_counter++;
        }
        else {
            if (line_iterator_peek(it) == COMMA_CHAR && comma_counter > 1) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_COMMA));
                return FALSE;
            }
            else if (!isalpha(line_iterator_peek(it))) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_OPERAND));
                return FALSE;
            }
            else if (isspace(line_iterator_peek(it))) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_WHITE_SPACE));
                return FALSE;
            }
            else if (line_iterator_peek(it) == COMMA_CHAR) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_COMMA));
                return FALSE;
            }
            else if (line_iterator_peek(it) == CLOSE_PAREN_CHAR || line_iterator_peek(it) == OPEN_PAREN_CHAR) {
                debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_PAREN));
                return FALSE;
            }
        }
    }

    if (args_count != 2) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_MISSING_OPERAND));
        return FALSE;
    }

    while (!line_iterator_is_end(it)) {
        if (line_iterator_peek(it) == CLOSE_PAREN_CHAR) {
            close_paren_counter++;
        }
        if (!line_iterator_match_any(it, " )")) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
            return FALSE;
        }

        line_iterator_advance(it);
    }

    if (close_paren_counter > 1) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_PAREN));
        return FALSE;
    }

    return TRUE;
}

/**
* Check if the next character is a label name. This is used to distinguish between labels that are separated by spaces and blank lines.
* 
* @param it
* 
* @return TRUE if the next character is a label name FALSE otherwise
*/
bool is_label_name(LineIterator* it)
{
    line_iterator_consume_blanks(it);

    if (!isalpha(line_iterator_peek(it))) {
        return FALSE;
    }

    while (!line_iterator_is_end(it) && !line_iterator_match_any(it, ",() ")) {
        if (!isalpha(line_iterator_peek(it)) && !isdigit(line_iterator_peek(it))) {
            return FALSE;
        }
        line_iterator_advance(it);
    }

    return TRUE;
}

/**
* Validate that the iterator is at the end of a label. This is used to determine whether or not we should stop at the end of an already - processed label or not.
* 
* @param it
* 
* @return true if \ a it is at the end of a label false otherwise
*/
bool validate_label_ending(LineIterator* it)
{
    line_iterator_consume_blanks(it);
    return line_iterator_is_end(it);
}

/**
* Check if we are looking for a heuristics to be used in register names.
* 
* @param it - Iterator on the line to check. Must be at the start of a register name.
* 
* @return true if it is false otherwise. This is a heuristic
*/
bool is_register_name_heuristic(LineIterator it)
{
    int len = 0;
    while (!line_iterator_is_end(&it) && line_iterator_peek(&it) != COMMA_CHAR && line_iterator_peek(&it) != CLOSE_PAREN_CHAR) {
        len++;
        line_iterator_advance(&it);
    }
    return len == 2;
}

/**
* Check if the next character is a register name. This is used to distinguish between register names and the ones that are part of C / C ++ code.
* 
* @param it
* 
* @return True if the next character is a register name false otherwise
*/
bool is_register_name(LineIterator* it)
{
    /* Skip the 'r' */
    line_iterator_advance(it);
    return REG_MIN_NUM <= line_iterator_peek(it) && line_iterator_peek(it) <= REG_MAX_NUM;
}

bool is_register_name_whole(LineIterator* it)
{
    bool charFlag = FALSE;
    bool numFlag = FALSE;

    charFlag = line_iterator_peek(it) == 'r';
    line_iterator_advance(it);
    numFlag = REG_MIN_NUM <= line_iterator_peek(it) && line_iterator_peek(it) <= REG_MAX_NUM;

    return charFlag && numFlag;
}

/**
* Verifies that the next character is a valid integer. This is done by examining the characters that begin with a digit followed by a comma or a comma and verifying that it is a valid integer.
* 
* @param it
* @param line - Line in which the string was read. Used for error reporting.
* @param seps - String of digits to be checked for validity.
* @param dbg_list - Debug list to be populated with nodes.
* 
* @return TRUE if the integer is valid FALSE otherwise. On failure the iterator is left pointing at the character that failed
*/
bool verify_int(LineIterator* it, long line, char* seps, debugList* dbg_list)
{
    if (line_iterator_peek(it) == NEG_SIGN_CHAR || line_iterator_peek(it) == POS_SIGN_CHAR) {
        line_iterator_advance(it);
    }
    else if (line_iterator_peek(it) == COMMA_CHAR) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_COMMA));
        return FALSE;
    }
    else if (!isdigit(line_iterator_peek(it))) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_INT));
        return FALSE;
    }

    while (!line_iterator_is_end(it) && !line_iterator_match_any(it, seps)) {
        if (!isdigit(line_iterator_peek(it))) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_INT));
            return FALSE;
        }
        line_iterator_advance(it);
    }

    return TRUE;
}

/**
* Get the syntax group of a name. This is used to distinguish groups of syntaxes that are different from each other.
* 
* @param name
* 
* @return @c SG_GROUP_1 if the name is a syntax group @c SG_GROUP_2
*/
SyntaxGroups get_syntax_group(char* name)
{
    if (!name)
        return SG_GROUP_INVALID;

    if (strcmp(name, "mov") == 0 || strcmp(name, "add") == 0 || strcmp(name, "sub") == 0)
        return SG_GROUP_1;
    if (strcmp(name, "cmp") == 0)
        return SG_GROUP_2;
    if (strcmp(name, "not") == 0 || strcmp(name, "clr") == 0 || strcmp(name, "inc") == 0 ||
        strcmp(name, "dec") == 0 || strcmp(name, "red") == 0)
        return SG_GROUP_3;
    if (strcmp(name, "rts") == 0 || strcmp(name, "stop") == 0)
        return SG_GROUP_4;
    if (strcmp(name, "jmp") == 0 || strcmp(name, "bne") == 0 || strcmp(name, "jsr") == 0)
        return SG_GROUP_5;
    if (strcmp(name, "prn") == 0)
        return SG_GROUP_6;
    if (strcmp(name, "lea") == 0)
        return SG_GROUP_7;

    return SG_GROUP_INVALID;
}

bool directive_exists(LineIterator* line) {
    return line_iterator_word_includes(line, ".string") ||
        line_iterator_word_includes(line, ".data") ||
        line_iterator_word_includes(line, ".extern") ||
        line_iterator_word_includes(line, ".entry");
}