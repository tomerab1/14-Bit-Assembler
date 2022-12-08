#ifndef ENCODING_H
#define ENCODING_H

#include "line_iterator.h"
#include "debug.h"
#include "memory.h"

void encode_dot_string(LineIterator* it, memoryBuffer* img);
void encode_dot_data(LineIterator* it, memoryBuffer* img);

#endif