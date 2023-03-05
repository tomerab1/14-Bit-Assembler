/** @file
*/

#include "memory.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

struct MemoryWord
{
	unsigned char mem[SIZEOF_MEMORY_WORD];
};

struct imageMemory
{
	int counter;
	MemoryWord memory[RAM_MEMORY_SZ];
};

struct memoryBuffer
{
	imageMemory* instruction_image;
	imageMemory* data_image;
};

memoryBuffer* memory_buffer_get_new()
{
    memoryBuffer* mem_buff = (memoryBuffer*)xmalloc(sizeof(memoryBuffer));

    mem_buff->instruction_image = image_memory_get_new();
    mem_buff->data_image = image_memory_get_new();

    return mem_buff;
}

imageMemory* image_memory_get_new()
{
    imageMemory* ram = (imageMemory*)xmalloc(sizeof(imageMemory));
    image_memory_init(ram);
    return ram;
}

void image_memory_init(imageMemory* mem)
{
    mem->counter = 0;
    memset(mem->memory, RAM_INIT_VAL, sizeof(MemoryWord) * RAM_MEMORY_SZ);
}

imageMemory* memory_buffer_get_data_img(memoryBuffer* memBuff)
{
    return memBuff->instruction_image;
}

imageMemory* memory_buffer_get_inst_img(memoryBuffer* memBuff)
{
    return memBuff->instruction_image;
}

int img_memory_get_counter(imageMemory* im)
{
    return im->counter;
}

void img_memory_set_counter(imageMemory* im, int cnt)
{
    im->counter += cnt;
}

MemoryWord* img_memory_get_memory(imageMemory* im)
{
    return im->memory;
}

unsigned char* memory_word_get_memory(MemoryWord* mw)
{
    return mw->mem;
}

void set_image_memory(imageMemory* mem, unsigned char byte, int flags)
{
    MemoryWord* curr_block = img_memory_get_memory(mem);

    /* Set the ERA bits to the current block.*/
    if (flags & FLAG_ERA)     set_era_bits(curr_block, byte);
    /* Set the source bits of the current block.*/
    if (flags & FLAG_SOURCE)  set_source_bits(curr_block, byte);
    /* Set the destination bits of the current block.*/
    if (flags & FLAG_DEST)    set_dest_bits(curr_block, byte);
    /* Set the opcode cell 1 bits.*/
    if (flags & FLAG_OPCODE1) set_opcode_cell_1_bits(curr_block, byte);
    /* Set the opcode cell 2 bits.*/
    if (flags & FLAG_OPCODE2) set_opcode_cell_2_bits(curr_block, byte);
    /* Set the parameter 1 bit.*/
    if (flags & FLAG_PARAM1)  set_param1_bits(curr_block, byte);
    /* Set the parameter 2 bits of the current block.*/
    if (flags & FLAG_PARAM2)  set_param2_bits(curr_block, byte);
}

/* Constans for internal usage. */
#define OFFSET_0 0
#define OFFSET_1 1

#define MASK_ERA     0x03
#define MASK_DEST    0x0c
#define MASK_SOURCE  0x30
#define MASK_OPCODE1 0xc0
#define MASK_OPCODE2 0x03
#define MASK_PARAM1  0x0c
#define MASK_PARAM2  0x30

void set_era_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_ERA;
}

void set_source_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_SOURCE;
}

void set_dest_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_DEST;
}

void set_opcode_cell_1_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_OPCODE1;
}

void set_opcode_cell_2_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_OPCODE2;
}

void set_param1_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM1;
}

void set_param2_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM2;
}
