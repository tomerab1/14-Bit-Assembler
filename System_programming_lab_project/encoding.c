/** @file
*/
#include "encoding.h"

struct VarData
{
	char* leftVar; /* Pointer to the start of the line. */
	EncodingTypes leftVarEncType;
	char* rightVar; /* Pointer to the current position in the line. */
	EncodingTypes rightVarEncType;
	char* label;
	EncodingTypes labelEncType;
	int total;
};

void encode_dot_string(LineIterator* it, memoryBuffer* img)
{
	/* Eat all blanks */
	line_iterator_consume_blanks(it);

	/* Eat quote */
	line_iterator_advance(it);

	while (line_iterator_peek(it) != QUOTE_CHAR) {
		set_image_memory(memory_buffer_get_data_img(img), line_iterator_peek(it), FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
		img_memory_set_counter(memory_buffer_get_data_img(img), img_memory_get_counter(memory_buffer_get_data_img(img)) + 1);
		line_iterator_advance(it);
	}
	set_image_memory(memory_buffer_get_data_img(img), BACKSLASH_ZERO, FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
	img_memory_set_counter(memory_buffer_get_data_img(img), img_memory_get_counter(memory_buffer_get_data_img(img)) + 1);
}

void encode_dot_data(LineIterator* it, memoryBuffer* img)
{
	char* word;

	while ((word = line_iterator_next_word(it, COMMA_STRING))) {
		unsigned int num = get_num(word);
		encode_integer(memory_buffer_get_data_img(img), num);

		/* Consume blanks and comma */
		line_iterator_consume_blanks(it);
		line_iterator_advance(it);

		/* Free the extracted word. */
		free(word);
	}
}

void encode_label_start_process(LineIterator* it, memoryBuffer* img, SymbolTable* symTable, debugList* dbg_list) {
	VarData* variables = NULL;
	char* opcode = NULL;
	SyntaxGroups synGroup;

	line_iterator_reset(it);
	line_iterator_jump_to(it, COLON_CHAR);

	opcode = line_iterator_next_word(it, SPACE_STRING);
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

	if (!variables) {
		free(opcode);
		return;
	}

	/*encode ARE missing*/
	encode_labels(variables, synGroup, symTable, memory_buffer_get_inst_img(img));
	varData_destroy(&variables);
	free(opcode);
}

void encode_integer(imageMemory* img, unsigned int num)
{
	/* Copy first 8 bits */
	set_image_memory(img, num & BYTE_MASK, FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);

	/* Copy second 8 bits */
	set_image_memory(img, (num & WORD_MASK) >> BITS_3_MASK, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
	img_memory_set_counter(img, img_memory_get_counter(img) + 1);
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
	img_memory_set_counter(img, img_memory_get_counter(img) + 1);
}

void encode_opcode(LineIterator* it, memoryBuffer* img)
{
	char* opcode = line_iterator_next_word(it, SPACE_STRING);
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
	char* operands[3] = { NULL };
	int i, num;

	operands[0] = source;
	operands[1] = dest;

	if (get_operand_kind(source) == KIND_REG && get_operand_kind(dest) == KIND_REG) {
		/* Bits 2 - 7 -> First register. Bits 8 - 13 -> Second register. */
		set_image_memory(img, (unsigned char)(*(source + 1) - '0'), FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
		set_image_memory(img, (*(dest + 1) - '0') << 2, FLAG_OPCODE1 | FLAG_DEST | FLAG_SOURCE);
		img_memory_set_counter(img, img_memory_get_counter(img) + 1);
		return;
	}

	for (i = 0; i < 2; i++) {
		OperandKind kind;

		if (operands[i]) {
			kind = get_operand_kind(operands[i]);

			switch (kind) {
			case KIND_IMM:
				num = get_num(operands[i] + 1); /* +1 to ignore the '#' */
				set_image_memory(img, num << 2, FLAG_DEST | FLAG_SOURCE | FLAG_OPCODE1);
				set_image_memory(img, num >> BITS_3_MASK, FLAG_PARAM1 | FLAG_PARAM2 | FLAG_OPCODE2);
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
			 default:
				break;
			}
			img_memory_set_counter(img, img_memory_get_counter(img) + 1);
		}
	}
}

VarData* varData_get_new()
{
	VarData* ptr = (VarData*)xmalloc(sizeof(VarData));

	ptr->leftVar = ptr->rightVar = ptr->label = NULL;
	ptr->total = 0;

	return ptr;
}

void varData_destroy(VarData** ptr)
{
	free((*ptr)->leftVar);
	free((*ptr)->rightVar);
	free((*ptr)->label);
	free(*ptr);
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

	source = line_iterator_next_word(it, COMMA_STRING);

	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, SPACE_STRING);

	/* Encode the first memory word. */
	encode_preceding_word(memory_buffer_get_inst_img(img), op, source, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(memory_buffer_get_inst_img(img), source, dest);

	free(source);
	free(dest);
}

void encode_syntax_group_2(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;

	source = line_iterator_next_word(it, COMMA_STRING);

	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, SPACE_STRING);

	/* Encode the first memory word. */
	encode_preceding_word(memory_buffer_get_inst_img(img), op, source, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(memory_buffer_get_inst_img(img), source, dest);

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
	encode_preceding_word(memory_buffer_get_inst_img(img), op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(memory_buffer_get_inst_img(img), NULL, dest);

	free(dest);
}

void encode_syntax_group_4(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Encodes rts and stop */
	/* Encode the first memory word. */
	encode_preceding_word(memory_buffer_get_inst_img(img), op, NULL, NULL, FALSE);
}

void encode_syntax_group_5(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;
	line_iterator_jump_to(it, OPEN_PAREN_CHAR);

	source = line_iterator_next_word(it, COMMA_STRING);

	line_iterator_advance(it);

	dest = line_iterator_next_word(it, CLOSED_PAREN_STRING);

	/* Encode the first memory word. */
	if (source && dest) {
		encode_preceding_word(memory_buffer_get_inst_img(img), op, source, dest, TRUE);
		img_memory_set_counter(memory_buffer_get_inst_img(img), img_memory_get_counter(memory_buffer_get_inst_img(img)) + 1);

		/* Encode the source and dest. */
		encode_source_and_dest(memory_buffer_get_inst_img(img), source, dest);
	}
	else {
		encode_preceding_word(memory_buffer_get_inst_img(img), op, dest, source, FALSE);
		img_memory_set_counter(memory_buffer_get_inst_img(img), img_memory_get_counter(memory_buffer_get_inst_img(img)) + 1);
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
	encode_preceding_word(memory_buffer_get_inst_img(img), op, NULL, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(memory_buffer_get_inst_img(img), NULL, dest);

	free(dest);
}

void encode_syntax_group_7(LineIterator* it, Opcodes op, memoryBuffer* img)
{
	/* Source operand can be immediate, register or label. */
	/* Dest operand can be register or label. */
	char* source = NULL, * dest = NULL;

	source = line_iterator_next_word(it, COMMA_STRING);

	line_iterator_consume_blanks(it);
	line_iterator_advance(it);

	dest = line_iterator_next_word(it, SPACE_STRING);

	/* Encode the first memory word. */
	encode_preceding_word(memory_buffer_get_inst_img(img), op, source, dest, FALSE);

	/* Encode the source and dest. */
	encode_source_and_dest(memory_buffer_get_inst_img(img), source, dest);

	free(source);
	free(dest);
}


VarData* extract_variables_group_1_and_2_and_7(LineIterator* it) {
	VarData* variablesData = varData_get_new();

	variablesData->leftVar = line_iterator_next_word(it, COMMA_STRING);

	/* skip to command and consume it */
	line_iterator_jump_to(it, COMMA_CHAR);

	variablesData->rightVar = line_iterator_next_word(it, SPACE_STRING);
	variablesData->total = TWO_VARIABLES;

	return variablesData;
}

VarData* extract_variables_group_3_and_6(LineIterator* it) {
	VarData* variablesData = varData_get_new();

	variablesData->leftVar = line_iterator_next_word(it, SPACE_STRING);
	variablesData->total = ONE_VAR;

	return variablesData;
}

VarData* extract_variables_group_5(LineIterator* it) {
	VarData* variablesData = varData_get_new();

	if (line_iterator_word_includes(it, OPEN_PAREN_STRING)) {
		variablesData->label = line_iterator_next_word(it, OPEN_PAREN_STRING);
		line_iterator_advance(it); /*skips left parenthesis*/
		variablesData->leftVar = line_iterator_next_word(it, COMMA_STRING);
		line_iterator_advance(it); /*skips comma*/
		variablesData->rightVar = line_iterator_next_word(it, CLOSED_PAREN_STRING);
		variablesData->total = TWO_VARS_ONE_LABEL;
	}
	else {
		variablesData->label = get_last_word(it);
		variablesData->total = ONE_VAR;
	}

	return variablesData;
}

char* varData_get_leftVar(VarData* vd)
{
	return vd->leftVar;
}

char* varData_get_rightVar(VarData* vd)
{
	return vd->rightVar;
}

char* varData_get_label(VarData* vd)
{
	return vd->label;
}

EncodingTypes varData_get_leftEncType(VarData* vd)
{
	return vd->leftVarEncType;
}

EncodingTypes varData_get_rgithEncType(VarData* vd)
{
	return vd->rightVarEncType;
}

EncodingTypes varData_get_labelEncType(VarData* vd)
{
	return vd->labelEncType;
}

int varData_get_total(VarData* vd)
{
	return vd->total;
}

void encode_labels(VarData* variables, SyntaxGroups synGroup, SymbolTable* symTable, imageMemory* img)
{
	SymbolTableNode* nodePtr = NULL;
	bool isDualRegister = FALSE;

	if (!variables)
		return;

	isDualRegister = (get_operand_kind(variables->leftVar) == KIND_REG && get_operand_kind(variables->rightVar) == KIND_REG);

	if (variables->label) {
		nodePtr = symbol_table_search_symbol(symTable, variables->label);
		if (nodePtr) {
			if (symbol_get_type(symbol_node_get_sym(nodePtr)) == SYM_EXTERN) {
				set_image_memory(img, ENCODING_EXT, FLAG_ERA);
			}
			else {
				set_image_memory(img, (symbol_get_counter(symbol_node_get_sym(nodePtr)) & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
				set_image_memory(img, (symbol_get_counter(symbol_node_get_sym(nodePtr)) << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
				set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
			}
		}
		img_memory_set_counter(img, img_memory_get_counter(img) + 1);
	}

	if (isDualRegister) {
		/* Must be a label with two register params. */
		/* 1 for the labels word, the other is a shared memory word for the 2 registers. */
		img_memory_set_counter(img, img_memory_get_counter(img) + 1);
	}
	else {
		if (variables->leftVar) {
			nodePtr = symbol_table_search_symbol(symTable, variables->leftVar);
			if (nodePtr) {
				if (symbol_get_type(symbol_node_get_sym(nodePtr)) == SYM_EXTERN) {
					set_image_memory(img, ENCODING_EXT, FLAG_ERA);
				}
				else {
					set_image_memory(img, (symbol_get_counter(symbol_node_get_sym(nodePtr)) & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
					set_image_memory(img, (symbol_get_counter(symbol_node_get_sym(nodePtr)) << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
					set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
				}
			}
			img_memory_set_counter(img, img_memory_get_counter(img) + 1);
		}

		if (variables->rightVar) {
			nodePtr = symbol_table_search_symbol(symTable, variables->rightVar);
			if (nodePtr) {
				if (symbol_get_type(symbol_node_get_sym(nodePtr)) == SYM_EXTERN) {
					set_image_memory(img, ENCODING_EXT, FLAG_ERA);
				}
				else {
					set_image_memory(img, ((symbol_get_counter(symbol_node_get_sym(nodePtr))) & 0xff) << 0x02, FLAG_OPCODE1 | FLAG_SOURCE | FLAG_DEST);
					set_image_memory(img, ((symbol_get_counter(symbol_node_get_sym(nodePtr))) << 0x02) >> 0x08, FLAG_OPCODE2 | FLAG_PARAM1 | FLAG_PARAM2);
					set_image_memory(img, ENCODING_RELOC, FLAG_ERA);
				}
			}
			img_memory_set_counter(img, img_memory_get_counter(img) + 1);
		}
	}
}
