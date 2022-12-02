#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "first_pass.h"
#include <string.h>
#include <ctype.h>

errorCodes check_label_syntax(const char* label)
{
    int i;
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

bool is_valid_label(const char* label)
{
    return (check_label_syntax(label) == ERROR_CODE_OK) ? TRUE : FALSE;
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

bool cmp_register_name(const char* str)
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

Opcodes get_opcode(const char* str)
{
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
        return validate_syntax_sym_def(&it, line, dbg_list);

    default:
        break;
    }

    return TRUE;
}

bool validate_syntax_sym_def(LineIterator* it, long line, debugList* dbg_list)
{
    const char* word;
    Opcodes opcode = OP_UNKNOWN;

    while ((word = line_iterator_next_word(it)) != NULL) {
        if ((opcode = get_opcode(word)) != OP_UNKNOWN) {
            switch (opcode) {
            case OP_MOV: return match_syntax_opcode_mov(it, line, dbg_list);
            case OP_CMP: return match_syntax_opcode_cmp(it, line, dbg_list);
            case OP_ADD: return match_syntax_opcode_add(it, line, dbg_list);
            case OP_SUB: return match_syntax_opcode_sub(it, line, dbg_list);
            case OP_NOT: return match_syntax_opcode_not(it, line, dbg_list);
            case OP_CLR: return match_syntax_opcode_clr(it, line, dbg_list);
            case OP_LEA: return match_syntax_opcode_lea(it, line, dbg_list);
            case OP_INC: return match_syntax_opcode_inc(it, line, dbg_list);
            case OP_DEC: return match_syntax_opcode_dec(it, line, dbg_list);
            case OP_JMP: return match_syntax_opcode_jmp(it, line, dbg_list);
            case OP_BNE: return match_syntax_opcode_bne(it, line, dbg_list);
            case OP_RED: return match_syntax_opcode_red(it, line, dbg_list);
            case OP_PRN: return match_syntax_opcode_prn(it, line, dbg_list);
            case OP_JSR: return match_syntax_opcode_jsr(it, line, dbg_list);
            case OP_RTS: return match_syntax_opcode_rts(it, line, dbg_list);
            case OP_STOP: return match_syntax_opcode_stop(it, line, dbg_list);
            }
        }
        /* Check syntax for .data/.string, otherwise it's an error. */
        free(word);
    }

    return TRUE;
}

/* Receives to operands, source op can be immediate integer, label or register */
bool match_syntax_opcode_mov(LineIterator* it, long line, debugList* dbg_list)
{

    /************************* Match operand 1 ****************************/

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

bool match_syntax_opcode_cmp(LineIterator* it, long line, debugList* dbg_list)
{
    /************************* Match operand 1 ****************************/

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

bool match_syntax_opcode_add(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_sub(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_not(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_clr(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_lea(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_inc(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_dec(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_jmp(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_bne(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_red(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_prn(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_jsr(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_rts(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_syntax_opcode_stop(LineIterator* it, long line, debugList* dbg_list)
{

}

bool match_operand(LineIterator* it, long line, int flags, debugList* dbg_list)
{
    switch (flags) {
    case FLAG_LABEL: 
        if (!is_label_name(it)) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_NAME));
            return FALSE;
        }
        break;
    case FLAG_NUMBER: 
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
        }
        break;
    default:
        break;
    }

    /* Comsume comma */
    line_iterator_jump_to(it, COMMA_CHAR);

    return TRUE;
}

bool match_syntax_opcode_dot_string(LineIterator* it, long line, debugList* dbg_list)
{
    return FALSE;
}

bool match_syntax_opcode_dot_data(LineIterator* it, long line, debugList* dbg_list)
{
    return FALSE;
}

bool is_label_name(LineIterator* it)
{
    while (!line_iterator_is_end(it) && (line_iterator_peek(it) != COMMA_CHAR)) {
        if (!isalpha(line_iterator_peek(it)) && !isdigit(line_iterator_peek(it))) {
            return FALSE;
        }
        line_iterator_advance(it);
    }
    return TRUE;
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

bool verify_int(LineIterator* it, long line, char* seps, debugList* dbg_list)
{
    if (line_iterator_peek(it) == NEG_SIGN_CHAR || line_iterator_peek(it) == POS_SIGN_CHAR) {
        line_iterator_advance(it);
    }

    while (!line_iterator_is_end(it) && line_iterator_match_any(it, seps)) {
        if (!isdigit(line_iterator_peek(it))) {
            debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_INT));
            return FALSE;
        }
        line_iterator_advance(it);
    }

    return TRUE;
}