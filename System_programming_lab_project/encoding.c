/** @file
*/

#include "encoding.h"

void encode_dot_string(LineIterator* it, memoryBuffer* img)
{
	/* Eat all blanks */
	line_iterator_consume_blanks(it);

	/* Eat quote */
	line_iterator_advance(it);

	while (line_iterator_peek(it) != QUOTE_CHAR) {
		set_image_memory(&img->data_image, line_iterator_peek(it), FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
		img->instruction_image.memory[img->instruction_image.counter].encodingCount++;
		img->data_image.counter++;
		line_iterator_advance(it);
	}
	set_image_memory(&img->data_image, '\0', FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
	img->instruction_image.memory[img->instruction_image.counter].encodingCount++;
	img->data_image.counter++;
}

void encode_dot_data(LineIterator* it, memoryBuffer* img)
{
	char* word;

	while ((word = line_iterator_next_word(it, ", "))) {
		unsigned int num = get_num(word);
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
	img->memory[img->counter].encodingCount++;
	img->counter++;
}

void encode_preceding_word(imageMemory* img, Opcodes op, char* source, char* dest, bool is_jmp_label)
{
	set_image_memory(img, op << OFFSET_OPCODE1, FLAG_OPCODE1);
	set_image_memory(img, (op >> 2) << OFFSET_OPCODE2, FLAG_OPCODE2);

	if (is_jmp_label) {
		set_image_memory(img, ADDRESSING_PARAM << OFFSET_DEST, FLAG_DEST);

		if (source) {
			if (*source == HASH_CHAR)
				set_image_memory(img, ADDRESSING_IMM << OFFSET_PARAM2, FLAG_PARAM2);
			else if (cmp_register_name(source))
				set_image_memory(img, ADDRESSING_REG << OFFSET_PARAM2, FLAG_PARAM2);
			else
				set_image_memory(img, ADDRESSING_DIR << OFFSET_PARAM2, FLAG_PARAM2);
		}
		if (dest) {
			if (*dest == HASH_CHAR)
				set_image_memory(img, ADDRESSING_IMM << OFFSET_PARAM1, FLAG_PARAM1);
			else if (cmp_register_name(dest))
				set_image_memory(img, ADDRESSING_REG << OFFSET_PARAM1, FLAG_PARAM1);
			else
				set_image_memory(img, ADDRESSING_DIR << OFFSET_PARAM1, FLAG_PARAM1);
		}
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
	img->memory[img->counter].encodingCount++;
	img->counter++;
}

void encode_opcode(LineIterator* it, SymbolTable* table, memoryBuffer* img)
{
	char* opcode = line_iterator_next_word(it, " ");
	Opcodes op = get_opcode(opcode);
	SyntaxGroups group = get_syntax_group(opcode);
	switch (group) {
	case SG_GROUP_1:
		table->completed ? encode_syntax_group_1(it, op, img,table): encode_syntax_group_1(it, op, img, NULL);
		break;
	case SG_GROUP_2: 
		table->completed ? encode_syntax_group_2(it, op, img, table) : encode_syntax_group_2(it, op, img, NULL);
		break;
	case SG_GROUP_3:
		table->completed ? encode_syntax_group_3(it, op, img, table) : encode_syntax_group_3(it, op, img, NULL);
		break;
	case SG_GROUP_4:
		table->completed ? encode_syntax_group_4(it, op, img, table) : encode_syntax_group_4(it, op, img, NULL);
		break;
	case SG_GROUP_5:
		table->completed ? encode_syntax_group_5(it, op, img, table) : encode_syntax_group_5(it, op, img, NULL);
		break;
	case SG_GROUP_6:
		table->completed ? encode_syntax_group_6(it, op, img, table) : encode_syntax_group_6(it, op, img, NULL);
		break;
	case SG_GROUP_7:
		table->completed ? encode_syntax_group_7(it, op, img, table) : encode_syntax_group_7(it, op, img, NULL);
		break;
	default: break;
	}

    free(opcode);
}

void encode_source_and_dest(imageMemory* img, char* source, char* dest, SymbolTable* table)
{
	char* operands[] = { source, dest, NULL };
	int i, num;

	if (get_operand_kind(source,table) == KIND_REG && get_operand_kind(dest,table) == KIND_REG) {
		/* Bits 2 - 7 -> First register. Bits 8 - 13 -> Second register. */
		set_image_memory(img, (unsigned char)(*(source + 1) - '0'), FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
		set_image_memory(img, (*(dest + 1) - '0') << 2, FLAG_OPCODE1 | FLAG_DEST | FLAG_SOURCE);
		img->memory[img->counter].encodingCount++;
		img->counter++;
		return;
	}

	for (i = 0; i < 2; i++) {
		OperandKind kind;
		int address;
		if (!operands[i]) 
			continue;
		 
		kind = get_operand_kind(operands[i], table);
		switch (kind) {
		case KIND_IMM:
			num = get_num(operands[i] + 1); /* +1 to ignore the '#' */
			set_image_memory(img, num << 2, FLAG_DEST | FLAG_SOURCE | FLAG_OPCODE1);
			set_image_memory(img, num >> 0x08, FLAG_PARAM1 | FLAG_PARAM2 | FLAG_OPCODE2);
			break;
		case KIND_LABEL:
			
			if (operands[i] == source) {
				address = symbol_table_search_symbol(table, source)->sym.counter;
				set_image_memory(img, *(img->memory[address].mem), FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
			}
			else if (operands[i] == dest) {
				address = symbol_table_search_symbol(table, dest)->sym.counter;
				set_image_memory(img, *(img->memory[address].mem) << 2, FLAG_OPCODE1 | FLAG_DEST | FLAG_SOURCE);
			}
			break;
		case KIND_REG:
			/* Two different cases for source and dest. */
			if (operands[i] == source) {
				set_image_memory(img, *(source + 1) - '0', FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
			}
			else if (operands[i] == dest) {
				set_image_memory(img, (*(dest + 1) - '0') << 2, FLAG_OPCODE1 | FLAG_DEST | FLAG_SOURCE);
			}
			break;
		}
		img->memory[img->counter].encodingCount++;
		img->counter++;
	}
}

OperandKind get_operand_kind(char* op, SymbolTable* table)
{
	if (!op) return KIND_NONE;
	if (*op == HASH_CHAR) return KIND_IMM;
	if (cmp_register_name(op)) return KIND_REG;
	if (strchr(op, OPEN_PAREN_CHAR)) return KIND_LABEL_PARAM;
	if(symbol_table_search_symbol(table,op)) return KIND_LABEL;
}

void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, *dest = NULL;
	
	source = line_iterator_next_word(it, ", ");
	
	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, " ");

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, source, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, source, dest, table);

	free(source);
	free(dest);
}

void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;

	source = line_iterator_next_word(it, ", ");

	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, " ");

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, source, dest, FALSE, NULL);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, source, dest,table);

	free(source);
	free(dest);
}

void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* dest = NULL;

	dest = set_dest_groups_3_and_6(it);

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, NULL, dest, table);

	free(dest);
}

void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Encodes rts and stop */
	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, NULL, FALSE);
}

/*TBD will be discussed 03/01*/
void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, *dest = NULL;
	if(!(table)){
	line_iterator_jump_to(it, OPEN_PAREN_CHAR);

	source = line_iterator_next_word(it, ",");

	line_iterator_advance(it);

	dest = line_iterator_next_word(it, ")");
	}else{
		if (line_iterator_includes(it, OPEN_PAREN_CHAR)) {
			
		}
		else {
			
		}
	}
	/* Encode the first memory word. */
	if (source && dest) {
		encode_preceding_word(&img->instruction_image, op, source, dest, TRUE);
		img->instruction_image.memory[img->instruction_image.counter].encodingCount++;
		img->instruction_image.counter++;

		/* Encode the source and dest. */
		encode_source_and_dest(&img->instruction_image, source, dest, table);
	}
	else {
		encode_preceding_word(&img->instruction_image, op, dest, source, FALSE, NULL);
		img->instruction_image.memory[img->instruction_image.counter].encodingCount++;
		img->instruction_image.counter++;
	}

	free(source);
	free(dest);
}

/*Should actully print?*/
void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* dest = NULL;

	dest = set_dest_groups_3_and_6(it);

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, NULL, dest, table);

	free(dest);
}

void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img, SymbolTable* table)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;

	source = line_iterator_next_word(it, ", ");

	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, " ");

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, source, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, source, dest, table);

	free(source);
	free(dest);
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

char* set_dest_groups_3_and_6(LineIterator* it) {
	char* tempItLocation = it->current;
	line_iterator_jump_to(it, '\0');
	line_iterator_backwards(it);
	if (*(it->current) == SPACE_CHAR) return line_iterator_next_word(it, SPACE_CHAR);
	else return line_iterator_next_word(it, "\0");
}