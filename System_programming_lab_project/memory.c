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

void set_image_memory(imageMemory* mem, const char byte, int flags)
{
    MemoryWord* curr_block = &(mem->memory[mem->counter]);

    if (flags & FLAG_ERA)     set_era_bits(curr_block, byte);
    if (flags & FLAG_SOURCE)  set_source_bits(curr_block, byte);
    if (flags & FLAG_DEST)    set_dest_bits(curr_block, byte);
    if (flags & FLAG_OPCODE1) set_opcode_cell_1_bits(curr_block, byte);
    if (flags & FLAG_OPCODE2) set_opcode_cell_2_bits(curr_block, byte);
    if (flags & FLAG_PARAM1)  set_param1_bits(curr_block, byte);
    if (flags & FLAG_PARAM2)  set_param2_bits(curr_block, byte);

    mem->counter++;
}

#define OFFSET_0 0x00
#define OFFSET_1 0x01

void set_era_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_ERA;
}

void set_source_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_SOURCE;
}

void set_dest_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_DEST;
}

void set_opcode_cell_1_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_OPCODE1;
}

void set_opcode_cell_2_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_OPCODE2;
}

void set_param1_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM1;
}

void set_param2_bits(MemoryWord* mem, const char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM2;
}

#undef OFFSET_0
#undef OFFSET_1