#include "utils.h"
#include "syntactical_analysis.h"
#include "constants.h"
#include <string.h>
#include <ctype.h>

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
    size_t read = 0;
    char ch;

    while ((ch = (char)fgetc(in)) != EOF && ch != '\n' && read < SOURCE_LINE_MAX_LENGTH) {
        read_buffer[read++] = ch;
    }

    if (ch == '\n' && read == 0) {
        return read_buffer;
    }

    /* If nothing was read from the file */
    if (read == 0 && ch == EOF) {
        free(read_buffer);
        return NULL;
    }

    if (is_line_only_blanks(read_buffer)) {
        /* Mark for later functions in the pipeline to ignore. */
        *read_buffer = '\0';
        return read_buffer;
    }

    return read_buffer;
}

bool is_line_only_blanks(const char* line)
{
    while (*line) {
        if (!isspace(*line)) 
            return FALSE;
        line++;
    }
    return TRUE;
}

char* get_copy_string(const char* str)
{
    char* res = (char*)xcalloc(strlen(str), sizeof(char));
    memcpy(res, str, sizeof(char) * strlen(str));
    return res;
}

unsigned int get_2s_complement(int n)
{
    return (~n) + 1;
}
