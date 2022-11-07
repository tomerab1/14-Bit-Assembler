#include "memory.h"
#include <string.h>

memoryBuffer memory_buffer_get_new()
{
    memoryBuffer mem_buff;

    mem_buff.instruction_image = image_memory_get_new();
    mem_buff.data_image = image_memory_get_new();
    memset(mem_buff.registers, REG_INIT_VAL, sizeof(Register) * REG_TOTAL);

    return mem_buff;
}

imageMemory image_memory_get_new()
{
    imageMemory ram;
    image_memory_init(&ram);
    return ram;
}

void image_memory_init(imageMemory* mem)
{
    mem->counter = 0;
    memset(mem->memory, RAM_INIT_VAL, sizeof(MemoryWord) * RAM_MEMORY_SZ);
}
