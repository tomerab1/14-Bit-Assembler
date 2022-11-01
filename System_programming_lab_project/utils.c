#include "utils.h"
#include "constants.h"
#include <string.h>

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

void* xrealloc(void* ptr, size_t alloc_sz)
{
    void* mem = realloc(ptr, alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

void* xcalloc(size_t count, size_t alloc_sz)
{
    void* mem = calloc(count, alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

void* xmalloc(size_t alloc_sz)
{
    void* mem = malloc(alloc_sz);
    if (!mem) printf("%s\n", "Error: memory allocation failed !");
    return mem;
}

const char* get_outfile_name(const char* path, const char* postfix)
{
    char* new_name = (char*)xcalloc(strlen(path) + strlen(postfix) + 1, sizeof(char));
    char* dot_loc = strrchr(path, POSTFIX_DOT_CHAR); /* Assuming path will contain a dot and then a postfix. */

    /* Will copy until the dot. */
    memmove(new_name, path, sizeof(char) * (dot_loc - path));
    strncat(new_name, postfix, strlen(postfix));

    return new_name;
}

FILE* open_file(const char* path, const char* mode)
{
    FILE* f = fopen(path, mode);

    if (!f) {
        printf("Error: Could not open %s for %s !\n", path, mode);
        exit(EXIT_FAILURE);
    }

    return f;
}

char* get_line(FILE* in)
{
    char* read_buffer = (char*)xcalloc(SOURCE_LINE_MAX_LENGTH + 1, sizeof(char));
    return fgets(read_buffer, SOURCE_LINE_MAX_LENGTH, in);
}
