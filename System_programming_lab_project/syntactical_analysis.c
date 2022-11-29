#include "syntactical_analysis.h"
#include "line_iterator.h"
#include <string.h>
#include <ctype.h>

errorCodes check_lable_syntax(const char* label)
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

