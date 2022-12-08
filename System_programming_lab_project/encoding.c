#include "encoding.h"

void encode_dot_string(LineIterator* it, memoryBuffer* img)
{
	/* Eat all blanks */
	line_iterator_consume_blanks(it);

	/* Eat quote */
	line_iterator_advance(it);

	while (line_iterator_peek(it) != QUOTE_CHAR) {
		set_image_memory(&img->data_image, line_iterator_peek(it), FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
		img->data_image.counter++;
		line_iterator_advance(it);
	}
	set_image_memory(&img->data_image, '\0', FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
	img->data_image.counter++;
}

void encode_dot_data(LineIterator* it, memoryBuffer* img)
{
	char* word;

	while ((word = line_iterator_next_word(it, ", "))) {
		unsigned int num;

		/* We can ignore the return value, if we reached this point, the syntax is valid. */
		(void) sscanf(word, "%u", &num);
		encode_integer(&img->data_image, num);

		/* Consume blanks and comma */
		line_iterator_consume_blanks(it);
		line_iterator_advance(it);

		/* Free the extracted word. */
		free(word);
	}
}

void encode_integer(imageMemory* img, unsigned int num)
{
	/* Copy first 8 bits */
	set_image_memory(img, num & 0xff, FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1); 

	/* Copy second 8 bits */
	set_image_memory(img, (num & 0xff00) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
	img->counter++;
}

void encode_preceding_word(imageMemory* img, Opcodes op, const char* source, const char* dest, bool is_jmp_label)
{
	set_image_memory(img, op << OFFSET_OPCODE1, FLAG_OPCODE1);
	set_image_memory(img, (op >> 2) << OFFSET_OPCODE2, FLAG_OPCODE2);

	if (is_jmp_label) {
		set_image_memory(img, ADDRESSING_PARAM << OFFSET_DEST, FLAG_DEST);

		if (*source == HASH_CHAR)
			set_image_memory(img, ADDRESSING_IMM << OFFSET_PARAM2, FLAG_PARAM2);
		else if (cmp_register_name(source))
			set_image_memory(img, ADDRESSING_REG << OFFSET_PARAM2, FLAG_PARAM2);
		else
			set_image_memory(img, ADDRESSING_DIR << OFFSET_PARAM2, FLAG_PARAM2);

		if (*dest == HASH_CHAR)
			set_image_memory(img, ADDRESSING_IMM << OFFSET_PARAM1, FLAG_PARAM1);
		else if (cmp_register_name(dest))
			set_image_memory(img, ADDRESSING_REG << OFFSET_PARAM1, FLAG_PARAM1);
		else
			set_image_memory(img, ADDRESSING_DIR << OFFSET_PARAM1, FLAG_PARAM1);
	}
	else {
		if (source) {
			if (*source == HASH_CHAR)
				set_image_memory(img, ADDRESSING_IMM << OFFSET_SOURCE, FLAG_SOURCE);
			else if (cmp_register_name(source))
				set_image_memory(img, ADDRESSING_REG << OFFSET_SOURCE, FLAG_SOURCE);
			else
				set_image_memory(img, ADDRESSING_DIR << OFFSET_SOURCE, FLAG_SOURCE);
		}

		if (dest) {
			if (*dest == HASH_CHAR)
				set_image_memory(img, ADDRESSING_IMM << OFFSET_DEST, FLAG_DEST);
			else if (cmp_register_name(dest))
				set_image_memory(img, ADDRESSING_REG << OFFSET_DEST, FLAG_DEST);
			else
				set_image_memory(img, ADDRESSING_DIR << OFFSET_DEST, FLAG_DEST);
		}
	}

	img->counter++;
}

void encode_opcode(LineIterator* it, memoryBuffer* img)
{
	const char* opcode = line_iterator_next_word(it, " ");
	Opcodes op = get_opcode(opcode);
	SyntaxGroups group = get_syntax_group(opcode);
	
	switch (group) {
	case SG_GROUP_1: encode_syntax_group_1(it, op, img);
	case SG_GROUP_2: encode_syntax_group_2(it, op, img);
	case SG_GROUP_3: encode_syntax_group_3(it, op, img);
	case SG_GROUP_4: encode_syntax_group_4(it, op, img);
	case SG_GROUP_5: encode_syntax_group_5(it, op, img);
	case SG_GROUP_6: encode_syntax_group_6(it, op, img);
	case SG_GROUP_7: encode_syntax_group_7(it, op, img);
	default: break;
	}
}

void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	const char* source = NULL, *dest = NULL;
		
	source = line_iterator_next_word(it, ", ");
	
	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, " ");

	encode_preceding_word(img, op, source, dest, FALSE);


	free(source);
	free(dest);
}

void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img)
{

}

void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img)
{

}

void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	
}

void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	const char* source = NULL, * dest = NULL;

	line_iterator_jump_to(it, OPEN_PAREN_CHAR);

	source = line_iterator_next_word(it, ",");

	line_iterator_advance(it);

	dest = line_iterator_next_word(it, ")");

	encode_preceding_word(&img->instruction_image, op, source, dest, TRUE);


	free(source);
	free(dest);
}

void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img)
{

}

void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img)
{

}

varData extract_variables_group_1_and_2_and_7(LineIterator* it, debugList* dbg_list) {
	varData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_word(it, ", ");
	/* skips comma */
	line_iterator_advance(it);
	variablesData.rightVar = line_iterator_next_word(it, " ");

	return variablesData;
}

varData extract_variables_group_3_and_6(LineIterator* it, Opcodes command, debugList* dbg_list) {
	varData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_word(it, " ");

	return variablesData;
}

varData extract_variables_group_5(LineIterator* it, Opcodes command, debugList* dbg_list) {
	char* nextWord = line_iterator_next_word(it, " ");
	varData variablesData = { NULL };
	if (line_iterator_includes(it, OPEN_PAREN_CHAR)) {
		variablesData.label = line_iterator_next_word(it, "( ");
		line_iterator_advance(it);//skips left parenthesis
		variablesData.leftVar = line_iterator_next_word(it, ", ");
		line_iterator_advance(it);//skips comma
		variablesData.rightVar = line_iterator_next_word(it, "( ");
	}
	else {
		variablesData.label = line_iterator_next_word(it, " ");
	}
	return variablesData;
}

