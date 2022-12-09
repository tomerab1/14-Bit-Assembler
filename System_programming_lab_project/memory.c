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
    MemoryWord* curr_block = &mem->memory[mem->counter];

    if (flags & FLAG_ERA)     set_era_bits(curr_block, byte);
    if (flags & FLAG_SOURCE)  set_source_bits(curr_block, byte);
    if (flags & FLAG_DEST)    set_dest_bits(curr_block, byte);
    if (flags & FLAG_OPCODE1) set_opcode_cell_1_bits(curr_block, byte);
    if (flags & FLAG_OPCODE2) set_opcode_cell_2_bits(curr_block, byte);
    if (flags & FLAG_PARAM1)  set_param1_bits(curr_block, byte);
    if (flags & FLAG_PARAM2)  set_param2_bits(curr_block, byte);
}

#define OFFSET_0 0
#define OFFSET_1 1

void set_era_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_ERA;
}

void set_source_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_SOURCE;
}

void set_dest_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_DEST;
}

void set_opcode_cell_1_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_OPCODE1;
}

void set_opcode_cell_2_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_OPCODE2;
}

void set_param1_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM1;
}

void set_param2_bits(MemoryWord* mem, const unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM2;
}

unsigned char get_era_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_ERA;
}

unsigned char get_source_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_SOURCE;
}

unsigned char get_dest_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_DEST;
}

unsigned char get_opcode_cell_1_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_OPCODE1;
}

unsigned char get_opcode_cell_2_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_OPCODE2;
}

unsigned char get_param1_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_PARAM1;
}

unsigned char get_param2_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_PARAM2;
}

#undef OFFSET_0
#undef OFFSET_1

void dump_memory(memoryBuffer* buf)
{
    MemoryWord*  inst = buf->instruction_image.memory,
                *data = buf->data_image.memory;
    int i, j;

    for (i = 0; i < buf->instruction_image.counter; i++) {
        unsigned int bits = (inst[i].mem[1] << 0x08) | (inst[i].mem[0]);

        if (bits == 0) {
            printf("?");
        }
        else {
            for (int i = 13; i >= 0; i--) {
                if (bits & (0x01 << i)) printf("1");
                else printf("0");
            }
        }

        printf("\n");
    }

    for (i = 0; i < buf->data_image.counter; i++) {
        unsigned int bits = (data[i].mem[1] << 0x08) | (data[i].mem[0]);

        for (int i = 13; i >= 0; i--) {
            if (bits & (0x01 << i)) printf("1");
            else printf("0");
        }

        printf("\n");
    }
}