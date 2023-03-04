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
		unsigned int num = get_num(word);
		encode_integer(&img->data_image, num);

		/* Consume blanks and comma */
		line_iterator_consume_blanks(it);
		line_iterator_advance(it);

		/* Free the extracted word. */
		free(word);
	}
}

void encode_label_start_process(LineIterator* it, memoryBuffer* img, SymbolTable* symTable, debugList* dbg_list) {
	VarData variables = { 0 };
	char* opcode = NULL;
	Opcodes op;
	SyntaxGroups synGroup;

	line_iterator_reset(it);
	line_iterator_jump_to(it, COLON_CHAR);

	opcode = line_iterator_next_word(it, " ");
	op = get_opcode(opcode);
	synGroup = get_syntax_group(opcode);

	if (synGroup == SG_GROUP_1 || synGroup == SG_GROUP_2 || synGroup == SG_GROUP_7) {
		variables = extract_variables_group_1_and_2_and_7(it);

	}
	else if (synGroup == SG_GROUP_3 || synGroup == SG_GROUP_6) {
		variables = extract_variables_group_3_and_6(it);
	}
	else if (synGroup == SG_GROUP_5) {
		variables = extract_variables_group_5(it);
	}

	/*encode ARE missing*/
	encode_labels(&variables, synGroup, symTable, &img->instruction_image, dbg_list, it);

	free(variables.label);
	free(variables.leftVar);
	free(variables.rightVar);
}

void encode_integer(imageMemory* img, unsigned int num)
{
	/* Copy first 8 bits */
	set_image_memory(img, num & 0xff, FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);

	/* Copy second 8 bits */
	set_image_memory(img, (num & 0xff00) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
	img->counter++;
}

/**
* Constans for the different offsets of the memory word fields.
*/
#define OFFSET_ERA     0x00
#define OFFSET_DEST    0x02
#define OFFSET_SOURCE  0x04
#define OFFSET_OPCODE1 0x06
#define OFFSET_OPCODE2 0x00
#define OFFSET_PARAM1  0x02
#define OFFSET_PARAM2  0x04

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
	img->counter++;
}

void encode_opcode(LineIterator* it, memoryBuffer* img)
{
	char* opcode = line_iterator_next_word(it, " ");
	Opcodes op = get_opcode(opcode);
	SyntaxGroups group = get_syntax_group(opcode);

	typedef void (*dispatchTable)(LineIterator*, Opcodes, memoryBuffer*);

	dispatchTable table[] = {
		encode_syntax_group_1, encode_syntax_group_2, encode_syntax_group_3,
		encode_syntax_group_4, encode_syntax_group_5, encode_syntax_group_6,
		encode_syntax_group_7
	};

	if (group != SG_GROUP_INVALID) table[group](it, op, img);


	free(opcode);
}

void encode_source_and_dest(imageMemory* img, char* source, char* dest)
{
	char* operands[] = { source, dest, NULL };
	int i, num;

	if (get_operand_kind(source) == KIND_REG && get_operand_kind(dest) == KIND_REG) {
		/* Bits 2 - 7 -> First register. Bits 8 - 13 -> Second register. */
		set_image_memory(img, (unsigned char)(*(source + 1) - '0'), FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
		set_image_memory(img, (*(dest + 1) - '0') << 2, FLAG_OPCODE1 | FLAG_DEST | FLAG_SOURCE);
		img->counter++;
		return;
	}

	for (i = 0; i < 2; i++) {
		OperandKind kind;
		int address;
		if (operands[i]) {
			kind = get_operand_kind(operands[i]);

			switch (kind) {
			case KIND_IMM:
				num = get_num(operands[i] + 1); /* +1 to ignore the '#' */
				set_image_memory(img, num << 2, FLAG_DEST | FLAG_SOURCE | FLAG_OPCODE1);
				set_image_memory(img, num >> 0x08, FLAG_PARAM1 | FLAG_PARAM2 | FLAG_OPCODE2);
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
			img->counter++;
		}
	}
}

OperandKind get_operand_kind(char* op)
{
	if (!op) return KIND_NONE;
	if (*op == HASH_CHAR) return KIND_IMM;
	if (cmp_register_name(op)) return KIND_REG;
	if (strchr(op, OPEN_PAREN_CHAR)) return KIND_LABEL_PARAM;
	return KIND_LABEL;
}

void encode_syntax_group_1(LineIterator* it, Opcodes op, memoryBuffer* img)
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
	encode_source_and_dest(&img->instruction_image, source, dest);

	free(source);
	free(dest);
}

void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img)
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
	encode_source_and_dest(&img->instruction_image, source, dest);

	free(source);
	free(dest);
}

void encode_syntax_group_3(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* dest = NULL;

	dest = get_last_word(it);

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, NULL, dest);

	free(dest);
}

void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Encodes rts and stop */
	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, NULL, FALSE);
}

void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;
	line_iterator_jump_to(it, OPEN_PAREN_CHAR);

	source = line_iterator_next_word(it, ",");

	line_iterator_advance(it);

	dest = line_iterator_next_word(it, ")");

	/* Encode the first memory word. */
	if (source && dest) {
		encode_preceding_word(&img->instruction_image, op, source, dest, TRUE);
		img->instruction_image.counter++;

		/* Encode the source and dest. */
		encode_source_and_dest(&img->instruction_image, source, dest);
	}
	else {
		encode_preceding_word(&img->instruction_image, op, dest, source, FALSE);
		img->instruction_image.counter++;
	}

	free(source);
	free(dest);
}

void encode_syntax_group_6(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* dest = NULL;

	dest = get_last_word(it);

	/* Encode the first memory word. */
	encode_preceding_word(&img->instruction_image, op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(&img->instruction_image, NULL, dest);

	free(dest);
}

void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img)
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
	encode_source_and_dest(&img->instruction_image, source, dest);

	free(source);
	free(dest);
}


VarData extract_variables_group_1_and_2_and_7(LineIterator* it) {
	VarData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_word(it, ", ");
	/* skip to command and consume it */
	line_iterator_jump_to(it, COMMA_CHAR);

	variablesData.rightVar = line_iterator_next_word(it, " ");

	variablesData.total = 2;
	return variablesData;
}

VarData extract_variables_group_3_and_6(LineIterator* it) {
	VarData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_word(it, " ");
	variablesData.total = 1;

	return variablesData;
}

VarData extract_variables_group_5(LineIterator* it) {
	VarData variablesData = { NULL };
	if (line_iterator_word_includes(it, "(")) {
		variablesData.label = line_iterator_next_word(it, "(");
		line_iterator_advance(it); /*skips left parenthesis*/
		variablesData.leftVar = line_iterator_next_word(it, ", ");
		line_iterator_advance(it); /*skips comma*/
		variablesData.rightVar = line_iterator_next_word(it, ")");
		variablesData.total = 3;
	}
	else {
		variablesData.label = get_last_word(it);
		variablesData.total = 1;
	}


	return variablesData;
}

void encode_labels(VarData* variables, SyntaxGroups synGroup, SymbolTable* symTable, imageMemory* img)
{
	SymbolTableNode* nodePtr = NULL;
	bool isDualRegister = (get_operand_kind(variables->leftVar) == KIND_REG && get_operand_kind(variables->rightVar) == KIND_REG);

	if (variables->label) {
		nodePtr = symbol_table_search_symbol(symTable, variables->label);
		if (nodePtr) {
			if (nodePtr->sym.type == SYM_EXTERN) {
				set_image_memory(img, ENCODING_EXT, FLAG_ERA);
			}
			else {
				set_image_memory(img, (nodePtr->sym.counter & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
				set_image_memory(img, (nodePtr->sym.counter << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
				set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
			}
		}
		img->counter++;
	}

	if (isDualRegister) {
		/* Must be a label with two register params. */
		img->counter++; /* 1 for the labels word, the other is a shared memory word for the 2 registers. */
	}
	else {
		if (variables->leftVar) {
			nodePtr = symbol_table_search_symbol(symTable, variables->leftVar);
			if (nodePtr) {
				if (nodePtr->sym.type == SYM_EXTERN) {
					set_image_memory(img, ENCODING_EXT, FLAG_ERA);
				}
				else {
					set_image_memory(img, (nodePtr->sym.counter & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
					set_image_memory(img, (nodePtr->sym.counter << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
					set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
				}
			}
			img->counter++;
		}

		if (variables->rightVar) {
			nodePtr = symbol_table_search_symbol(symTable, variables->rightVar);
			if (nodePtr) {
				if (nodePtr->sym.type == SYM_EXTERN) {
					set_image_memory(img, ENCODING_EXT, FLAG_ERA);
				}
				else {
					set_image_memory(img, (nodePtr->sym.counter & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
					set_image_memory(img, (nodePtr->sym.counter << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
					set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
				}
			}
			img->counter++;
		}
	}
}