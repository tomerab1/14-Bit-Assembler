#include "syntactical_analysis.h"
#include "line_iterator.h"
#include "first_pass.h"
#include <string.h>
#include <ctype.h>

errorCodes check_label_syntax(const char* label)
{
    int i;
    char* colon_loc = strrchr(label, ':');

    if (!isalpha(*label))
        return ERROR_CODE_INVALID_LABEL_DEF;

    if (colon_loc == NULL)
        return ERROR_CODE_SYNTAX_ERROR;

    if (isspace(*(colon_loc - 1)))
        return ERROR_CODE_SPACE_BEFORE_COLON;

    while (label < colon_loc) {
        if (!isalpha(*label) && !isdigit(*label))
            return ERROR_CODE_INVALID_CHAR_IN_LABEL;
        label++;
    }

    trim_symbol_name(label);
    if (is_register_name(label))
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

bool is_register_name(const char* str)
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
            switch (get_instruction_group(word)) {
            case IG_GROUP_0:
                return check_syntax_group_zero(it, line, dbg_list);
            case IG_GROUP_1:
                return check_syntax_group_one(it, line, opcode, dbg_list);
            case IG_GROUP_2:
                return check_syntax_group_two(it, line, opcode, dbg_list);
            default:
                /* ERROR */
                return FALSE;
            }
        }
        /* Check syntax for .data/.string, otherwise it's an error. */
        free(word);
    }

    return TRUE;
}

bool check_syntax_group_zero(LineIterator* it, long line, debugList* dbg_list)
{
    char* next = line_iterator_next_word(it);
    if (next != NULL) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_AMOUNT_OF_OPERANDS));
        free(next);
        return FALSE;
    }
    
    free(next);

    return TRUE;
}

bool check_syntax_group_one(LineIterator* it, long line, Opcodes opcode, debugList* dbg_list)
{
    char* word = line_iterator_next_word(it);
    AddressingGroups ad_group = classify_to_addressing_group(word);

    /* Only the 'prn' opcode can have addressing group zero in group one. */
    if (ad_group ==  AG_GROUP_0 && opcode != OP_PRN) {
        return FALSE;
    }

    switch (ad_group) {
    case AG_GROUP_0: return match_addressing_group_zero(it, word, line, dbg_list);
    case AG_GROUP_1: return match_addressing_group_one(it, word, line, dbg_list);
    case AG_GROUP_2: return match_addressing_group_two(it, word, line, dbg_list);
    case AG_GROUP_3: return match_addressing_group_three(it, word, line, dbg_list);
    default: return FALSE;
    }


    free(word);
    return TRUE;
}

bool check_syntax_group_two(LineIterator* it, long line, Opcodes opcode, debugList* dbg_list)
{
    char* word = line_iterator_next_word(it);
    match_addressing_group_zero(it, word + 1, line, dbg_list);

    free(word);
    return TRUE;
}

InstructionGroup get_instruction_group(const char* str)
{
    if (strcmp(str, "rts") == 0 || strcmp(str, "stop") == 0)
        return IG_GROUP_0;
 
    if (strcmp(str, "not") == 0 || strcmp(str, "clr") == 0 ||
        strcmp(str, "inc") == 0 || strcmp(str, "dec") == 0 ||
        strcmp(str, "jmp") == 0 || strcmp(str, "bne") == 0 ||
        strcmp(str, "red") == 0 || strcmp(str, "prn") == 0 ||
        strcmp(str, "jsr") == 0)
        return IG_GROUP_1;

    if (strcmp(str, "mov") == 0 || strcmp(str, "cmp") == 0 ||
        strcmp(str, "add") == 0 || strcmp(str, "sub") == 0 ||
        strcmp(str, "lea") == 0)
        return IG_GROUP_2;

    return IG_GROUP_INVALID;
}

bool match_addressing_group_zero(LineIterator* it, const char* word, long line, debugList* dbg_list)
{
    // Find the commas location.
    char* comma_loc = strchr(word, ',');

    if (comma_loc == NULL) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_MISSING_COMMA));
        return FALSE;
    }

    if (!verify_int(word, comma_loc)) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_VALUE_ERROR));
        return FALSE;
    }

    /* Advance word by the distance between the beginning and the first comma. */
    word += (comma_loc - word) + 1;
    comma_loc = strchr(word, ',');

    if (comma_loc != NULL) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_EXTRA_COMMA));
        return FALSE;
    }

    if (!is_register_name(word)) {
        debug_list_register_node(dbg_list, debug_list_new_node(it->start, it->current, line, ERROR_CODE_INVALID_NAME));
        return FALSE;
    }

    return TRUE;
}

bool match_addressing_group_one(LineIterator* it, const char* word, long line, debugList* dbg_list)
{

}

bool match_addressing_group_two(LineIterator* it, const char* word, long line, debugList* dbg_list)
{

}

bool match_addressing_group_three(LineIterator* it, const char* word, long line, debugList* dbg_list)
{

}

bool is_matching_adressing_group_zero(const char* word)
{
    const char* hash_loc = strchr(word, '#');
    word += (hash_loc - word) + 1; /* Advance by the difference. */

    if (strchr(word, '#') != NULL)
        return FALSE; /* More than one '#' */

    const char* comma_loc = strchr(word, ',');
    word += (comma_loc - word) + 1; /* Advance by the difference. */

    if (strchr(word, ',') != NULL) {
        return FALSE; /* Morethan one ',' */
    }

    return TRUE;
}

bool is_matching_adressing_group_one(const char* word)
{
    /* Check if the label starts with a valid char. */
    if (!isalpha(*word))
        return FALSE;

    while (*word != '\0') {
        /* A valid label name contains digits and chars only. */
        if (!isalpha(*word) && !isdigit(*word))
            return FALSE;
        word++;
    }
    return TRUE;
}

bool is_matching_adressing_group_two(const char* word)
{
    const char* open_paren_loc = strchr(word, '('), *comma_loc = strchr(word, ','),
               *close_paren_loc = strchr(word, ')');

    /* Check if the label starts with a valid char. */
    if (!isalpha(*word))
        return FALSE;

    /* Must contain a parentheses */
    if (open_paren_loc == NULL)
        return FALSE;
    /* Must contain a comma, and the comma must be after the first parentheses. */
    if (comma_loc == NULL || (comma_loc < open_paren_loc))
        return FALSE;
    /* Must contain a closing parentheses, and the parentheses must be after the comma. */
    if (close_paren_loc == NULL || (close_paren_loc < comma_loc))
        return FALSE;

    while (word < open_paren_loc) {
        /* A valid label name contains digits and chars only. */
        if (!isalpha(*word) && !isdigit(*word))
            return FALSE;
        word++;
    }

    return TRUE;
}

bool is_matching_adressing_group_three(const char* word)
{
    const char* comma_loc = strchr(word, ',');
    
    /* Must contain a comma between the operands*/
    if (comma_loc == NULL)
        return FALSE;
    word += (comma_loc - word) + 1; /* Advance by the diff */

    /* Check for extra comma. */
    return (strchr(word, ',') != NULL) ? FALSE : TRUE;
}


AddressingGroups classify_to_addressing_group(const char* word)
{
    if (is_matching_adressing_group_zero(word))
        return AG_GROUP_0;
    if (is_matching_adressing_group_one(word))
        return AG_GROUP_1;
    if (is_matching_adressing_group_two(word))
        return AG_GROUP_2;
    if (is_matching_adressing_group_three(word))
        return AG_GROUP_3;

    return AG_GROUP_UNKNOWN;
}

bool verify_int(char* word, char* other)
{
    /* If negative skip '-' */
    word += ((*word) == '-') ? 1 : 0;
    while (word < other) {
        if (!isdigit(*word)) return FALSE;
        word++;
    }

    return TRUE;
}