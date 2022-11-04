#include "memory.h"
#include <string.h>

CPU get_new_cpu()
{
    CPU cpu;

    cpu.ram = get_new_ram();
    cpu.DC = 0;
    memset(cpu.registers, REG_INIT_VAL, sizeof(Register) * REG_TOTAL);

    return cpu;
}

RAM get_new_ram()
{
    RAM ram;
    init_ram(&ram);
    return ram;
}

void init_ram(RAM* ram)
{
    memset(ram->memory, RAM_INIT_VAL, sizeof(MemoryWord) * RAM_MEMORY_SZ);
}
