#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SOURCE_LINE_MAX_LENGTH 81
#define LABEL_MAX_LENGTH 31

#define RAM_MEMORY_SZ 256
#define RAM_INIT_VAL 0
#define REG_INIT_VAL 0
#define INIT_PHY_SZ 4
#define INIT_LOG_SZ 0

#define SINGLE_ORDER_SIZE 14
#define E_R_A_SIZE 2
#define SOURCE_SIZE 2
#define DEST_SIZE 2
#define OPCODE_SIZE 4
#define PARAM_ONE_SIZE 2
#define PARAM_TWO_SIZE 2

#define END_LABEL_CHAR ':'
#define START_COMMENT_CHAR ';'

#define START_MACRO_DEF_CHAR 'm'
#define END_MACRO_DEF_CHAR 'e'

#define POSTFIX_DOT_CHAR '.'
#define START_MACRO_DEF_LEN 3
#define START_MACRO_DEFENITION "mcr"
#define END_MACRO_DEF_LEN 5
#define END_MACRO_DEFENITION "endmcr"

#define MODE_READ "r"
#define MODE_READ_WRITE "r+"
#define MODE_WRITE "w"
#define MODE_WRITE_READ "w+"
#define MODE_APP_READ "a+"


#define DOT_COMMAND '.'
#define DOT_DATA "data"
#define DOT_STRING "string"
#define DOT_ENTRY "entry"
#define DOT_EXTERN "extern"

#define LETTER_A 'A'
#define LETTER_Z 'Z'
#define COLON ':'

#define OBJECT_PRINT_DOT '.'
#define OBJECT_PRINT_SLASH '/'
#endif