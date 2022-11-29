#include "syntactical_analysis.h"
#include "line_iterator.h"
#include <string.h>
#include <ctype.h>

errorCodes check_label_syntax(const char* label)
{
    int i;
    char* colon_loc = strrchr(label, ':');

    if (colon_loc && isspace(*(colon_loc - 1)))
        return ERROR_CODE_SPACE_BEFORE_COLON;

    for (i = 0; label[i]; i++) {
        if (!isalpha(label[i]) && !isdigit(label[i]))
            return ERROR_CODE_INVALID_CHAR_IN_LABEL;
    }

    return ERROR_CODE_OK;
}

bool is_valid_label(const char* label)
{
    return (is_valid_label(label) == ERROR_CODE_OK) ? TRUE : FALSE;
}

bool verify_command_syntax(LineIterator* it, debugList* dbg_list)
{
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