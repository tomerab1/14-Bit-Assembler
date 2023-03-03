/** @file
*/

#include "memory.h"
#include <string.h>
#include <stdio.h>

/**
* Allocate a memory buffer to store instructions and data.
* The memory buffer must be freed with memory_buffer_free() when no longer needed.
* @return The memory buffer that was allocated with memory_buffer_free ()
*/
/**
* Allocate a memory buffer to store instructions and data. The memory buffer must be freed with memory_buffer_free () when no longer needed.
* 
* 
* @return The memory buffer that was allocated with memory_buffer_free () or NULL if there was insufficient memory
*/
memoryBuffer memory_buffer_get_new()
{
    memoryBuffer mem_buff;

    mem_buff.instruction_image = image_memory_get_new();
    mem_buff.data_image = image_memory_get_new();
    return mem_buff;
}

/**
* Create a new image memory object. This is a low - level function that should be used in place of imageMemory_new () to create a new image memory object.
* 
* 
* @return The newly created image memory object or NULL if an error occurred ( in which case you should call image_memory_get_err () to get the error message
*/
imageMemory image_memory_get_new()
{
    imageMemory ram;
    image_memory_init(&ram);
    return ram;
}

/**
* Initialize the image memory. This is called at the beginning of each program to initialize the image memory to a sensible state.
* 
* @param mem
*/
void image_memory_init(imageMemory* mem)
{
    mem->counter = 0;
    memset(mem->memory, RAM_INIT_VAL, sizeof(MemoryWord) * RAM_MEMORY_SZ);
}

/**
* Sets bits in image memory. This is used to read / write image memory from / to / source / destination files
* 
* @param mem
* @param byte - Byte to be written to bit 0
* @param flags - Flags to set bits
*/
void set_image_memory(imageMemory* mem, unsigned char byte, int flags)
{
    MemoryWord* curr_block = &mem->memory[mem->counter];

    // Set the ERA bits to the current block.
    if (flags & FLAG_ERA)     set_era_bits(curr_block, byte);
    // Set the source bits of the current block.
    if (flags & FLAG_SOURCE)  set_source_bits(curr_block, byte);
    // Set the destination bits of the current block.
    if (flags & FLAG_DEST)    set_dest_bits(curr_block, byte);
    // Set the opcode cell 1 bits.
    if (flags & FLAG_OPCODE1) set_opcode_cell_1_bits(curr_block, byte);
    // Set the opcode cell 2 bits.
    if (flags & FLAG_OPCODE2) set_opcode_cell_2_bits(curr_block, byte);
    // Set the parameter 1 bit.
    if (flags & FLAG_PARAM1)  set_param1_bits(curr_block, byte);
    // Set the parameter 2 bits of the current block.
    if (flags & FLAG_PARAM2)  set_param2_bits(curr_block, byte);
}

#define OFFSET_0 0
#define OFFSET_1 1

/**
* Set eras bit in memory. This is used to write ERA bits in PIC code.
* 
* @param mem
* @param byte - The byte to write
*/
void set_era_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_ERA;
}

/**
* Set the source bits in a byte. This is used to determine if we are going to read or write the data to / from the source register.
* 
* @param mem
* @param byte - The byte to set
*/
void set_source_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_SOURCE;
}

/**
* Set the DEST bit in a byte. This is used to indicate that we are going to write the destination to an address that is in the middle of a memory block.
* 
* @param mem
* @param byte - The byte to write
*/
void set_dest_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_DEST;
}

/**
* Set the 1 - bit opcode cell. This is used to indicate that a cell is in the middle of a instruction.
* 
* @param mem
* @param byte - The byte to set the cell to. Must be at least 0
*/
void set_opcode_cell_1_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_0] |= byte & MASK_OPCODE1;
}

/**
* Set the 2 - bit opcode cell. This is used to indicate that a cell is in the middle of a instruction.
* 
* @param mem
* @param byte - The byte to set
*/
void set_opcode_cell_2_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_OPCODE2;
}

/**
* Set parameter 1 bits. This is used to store parameters that are stored in the LSB first.
* 
* @param mem
* @param byte - The byte to store
*/
void set_param1_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM1;
}

/**
* Set parameter 2 bits. This is used to program the parameters that are stored in the FPGA
* 
* @param mem
* @param byte - The parameter 2 byte to be programmed in
*/
void set_param2_bits(MemoryWord* mem, unsigned char byte)
{
    mem->mem[OFFSET_1] |= byte & MASK_PARAM2;
}

/**
* Get ERA bits from memory. This is used to determine if we are going to write a fault or not.
* 
* @param mem
*/
unsigned char get_era_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_ERA;
}

/**
* Get the source bits. This is used to determine whether or not a function is inlined. If it is inlined we'll have to look at the stack pointer to see if it's out of bounds.
* 
* @param mem
*/
unsigned char get_source_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_SOURCE;
}

/**
* Get the number of bits set in the destination. This is used to determine if we are going to write a copy of the destination to an external memory location
* 
* @param mem
*/
unsigned char get_dest_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_DEST;
}

/**
* Get the number of 1 - bits in the opcode cell. This is used to determine if we are dealing with a cell that is being modified.
* 
* @param mem
*/
unsigned char get_opcode_cell_1_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_0] & MASK_OPCODE1;
}

/**
* Get the number of bits in the opcode cell 2. This is used to figure out if we are dealing with a 2 - byte opcode
* 
* @param mem
*/
unsigned char get_opcode_cell_2_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_OPCODE2;
}

/**
* Get parameter 1 bits. This is used to determine whether or not we are going to use the param1 register.
* 
* @param mem
*/
unsigned char get_param1_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_PARAM1;
}

/**
* Get parameter 2 bits. This is used to determine whether or not we are going to use the param1 register or not
* 
* @param mem
*/
unsigned char get_param2_bits(MemoryWord* mem)
{
    return mem->mem[OFFSET_1] & MASK_PARAM2;
}

#undef OFFSET_0
#undef OFFSET_1

/**
* Dump the contents of the memory buffer to stdout. 
* This is used for debugging purposes to ensure that the program doesn't accidentally end up corrupting the state of the system.
* @param buf
*/
void dump_memory(memoryBuffer* buf)
{
    MemoryWord*  inst = buf->instruction_image.memory,
                *data = buf->data_image.memory;
    int i, j;

    // Prints the instructions in the instruction image.
    for (i = 0; i < buf->instruction_image.counter; i++) {
        unsigned int bits = (inst[i].mem[1] << 0x08) | (inst[i].mem[0]);

        // Prints the bits of the bit set.
        if (bits == 0) {
            printf("?");
        }
        else {
            // Prints 1 or 0 if the bit is 1 or 0
            for (j = 13; j >= 0; j--) {
                // Prints 1 if bit 0x01 is set to 1 if bit 0x01 is set to 1 if bit 0x01 is set to 0x01
                if (bits & (0x01 << j)) printf("1");
                else printf("0");
            }
        }

        printf("\n");
    }

    // Prints the data image.
    for (i = 0; i < buf->data_image.counter; i++) {
        unsigned int bits = (data[i].mem[1] << 0x08) | (data[i].mem[0]);

        // Prints 1 or 0 if the bit is 1 or 0
        for (j = 13; j >= 0; j--) {
            // Prints 1 if bit 0x01 is set to 1 if bit 0x01 is set to 1 if bit 0x01 is set to 0x01
            if (bits & (0x01 << j)) printf("1");
            else printf("0");
        }

        printf("\n");
    }
}