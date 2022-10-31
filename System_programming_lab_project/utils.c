#include "utils.h"
#include "constants.h"

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
