/** @file
*/

#include "syntactical_analysis.h"
#include "line_iterator.h"
#include <string.h>
#include <ctype.h>

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

bool is_valid_label(char* label)
{
    return (check_label_syntax(label) == ERROR_CODE_OK) ? TRUE : FALSE;
}


bool isLabel(LineIterator* line) {
    return is_valid_label(line_iterator_next_word(line, " "));
}

bool verify_command_syntax(LineIterator* it, debugList* dbg_list)
{
    return FALSE;
}

void trim_symbol_name(char* sym)
{
    /* The ':' must be coupled with the name*/
    sym[strlen(sym) - 1] = '\0';
}

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

int extract_sentence_type(LineIterator* it) {
    it->current = it->start;
    /*empty sentence*/
    if (line_iterator_is_end(it)) {
        it->current = it->start;
        return EMPTY_SENTENCE;
    }
    if (directive_exists_basic(it)) return DIRECTIVE_SENTENCE; /*directive sentence*/
}

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

bool find_if_instruction_exists(LineIterator* line) {
    if (isLabel(line)) {
        skip_label_basic(line);
        int localOpcode = get_opcode(line_iterator_next_word(line, " "));

        if (localOpcode != 16)
            return TRUE;
    }
    return FALSE;
}

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

bool validate_syntax_extern_and_entry(LineIterator* it, long line, debugList* dbg_list)
{
    line_iterator_consume_blanks(it);

    if (!line_iterator_is_end(it)) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_TEXT_AFTER_END));
        return FALSE;
    }

    return TRUE;
}

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


bool validate_label_ending(LineIterator* it)
{
    line_iterator_consume_blanks(it);
    return line_iterator_is_end(it);
}

bool is_register_name_heuristic(LineIterator it)
{
    int len = 0;
    while (!line_iterator_is_end(&it) && line_iterator_peek(&it) != COMMA_CHAR && line_iterator_peek(&it) != CLOSE_PAREN_CHAR) {
        len++;
        line_iterator_advance(&it);
    }
    return len == 2;
}

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