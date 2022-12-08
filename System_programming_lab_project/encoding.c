#include "encoding.h"

void encode_dot_string(LineIterator* it, memoryBuffer* img)
{
	/* Eat all blanks */
	line_iterator_consume_blanks(it);

	/* Eat quote */
	line_iterator_advance(it);

	while (line_iterator_peek(it) != QUOTE_CHAR) {
		set_image_memory(&img->data_image, line_iterator_peek(it), FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
		line_iterator_advance(it);
	}
	set_image_memory(&img->data_image, '\0', FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
}

void encode_dot_data(LineIterator* it, memoryBuffer* img)
{
	char* word;

	while ((word = line_iterator_next))

}

