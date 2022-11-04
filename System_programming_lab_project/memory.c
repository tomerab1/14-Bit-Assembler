#include "memory.h"
#include <string.h>

memoryBuffer get_new_memory_buffer()
{
    memoryBuffer mem_buff;

    mem_buff.instruction_image = get_new_image_memory();
    mem_buff.data_image = get_new_image_memory();
    memset(mem_buff.registers, REG_INIT_VAL, sizeof(Register) * REG_TOTAL);

    return mem_buff;
}

imageMemory get_new_image_memory()
{
    imageMemory ram;
    init_image_memory(&ram);
    return ram;
}

void init_image_memory(imageMemory* mem)
{
    mem->counter = 0;
    memset(mem->memory, RAM_INIT_VAL, sizeof(MemoryWord) * RAM_MEMORY_SZ);
}
