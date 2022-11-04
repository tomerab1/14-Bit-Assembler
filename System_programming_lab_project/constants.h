#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SOURCE_LINE_MAX_LENGTH 80
#define LABEL_MAX_LENGTH 30

#define RAM_MEMORY_SZ 256
#define RAM_INIT_VAL 0
#define REG_INIT_VAL 0
#define INIT_PHY_SZ 4
#define INIT_LOG_SZ 0

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

#endif