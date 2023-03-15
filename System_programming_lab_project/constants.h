#ifndef CONSTANTS_H
#define CONSTANTS_H

/** @file
*/

#define SOURCE_LINE_MAX_LENGTH 81
#define LABEL_MAX_LENGTH 31
#define DECIMAL_ADDRESS_BASE 100

/*Debug.c*/
#define DEBUG_LINE_MAX_LENGTH 80

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

#define LETTER_A 'A'
#define LETTER_Z 'Z'
#define COLON ':'

#define OBJECT_PRINT_DOT '.'
#define OBJECT_PRINT_SLASH '/'
#define BACKSLASH_ZERO '\0'

/*Encoding.c*/
#define COMMA_STRING ", "
#define SPACE_STRING " "
#define OPEN_PAREN_STRING "("
#define OPEN_PAREN_STRING_W_SPACE "( "
#define CLOSED_PAREN_STRING ") "
#define COLON_STRING ":"
#define OFFSET_LABELS 0x02
#define BYTE_MASK 0xff
#define WORD_MASK 0xff00
#define START_OFFSET_SECOND_BYTE 0x08

/**
* Constans for the different offsets of the memory word fields.
*/
#define OFFSET_ERA     0x00
#define OFFSET_DEST    0x02
#define OFFSET_SOURCE  0x04
#define OFFSET_OPCODE1 0x06
#define OFFSET_OPCODE2 0x00
#define OFFSET_PARAM1  0x02
#define OFFSET_PARAM2  0x04

#define ONE_VAR 1
#define TWO_VARIABLES 2
#define TWO_VARS_ONE_LABEL 3

/*First Pass*/
#define DOT_EXTERN_STRING ".extern"
#define EXTERN_STRING "extern"
#define DOT_ENTRY_STRING ".entry"
#define ENTRY_STRING "entry"
#define DOT_DATA_STRING ".data"
#define DOT_STRING_STRING ".string"

/*Pre assembler*/
#define PRE_ASSEMBLER_FILE_EXTENSTION ".am"
#define SRC_ASSEMBLER_FILE_EXTENSTION ".txt"
#define EXTERN_ASSEMBLER_FILE_EXTENSTION ".extern"
#define ENTRY_ASSEMBLER_FILE_EXTENSTION ".entry"

#endif