#include "encoding.h"

void encode_dot_string(LineIterator* it, memoryBuffer* img, debugList* dbg_list)
{
	/* Eat all blanks */
	line_iterator_consume_blanks(it);

	/* Eat quote */
	line_iterator_advance(it);

	while (line_iterator_peek(it) != QUOTE_CHAR) {
		set_image_memory(&img->data_image, line_iterator_peek(it), FLAG_ERA | FLAG_SOURCE | FLAG_DEST | FLAG_OPCODE1);
		line_iterator_advance(it);
	}

}


varData extract_variables_group_1_and_2_and_7(LineIterator* it, debugList* dbg_list) {
	varData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_until_comma(it);
	line_iterator_advance(it);//skips comma
	variablesData.rightVar = line_iterator_next_word(it);

	return variablesData;
}

varData extract_variables_group_3_and_6(LineIterator* it, Opcodes command, debugList* dbg_list) {
	varData variablesData = { NULL };
	variablesData.leftVar = line_iterator_next_word(it);

	return variablesData;
}

varData extract_variables_group_5(LineIterator* it, Opcodes command, debugList* dbg_list) {
	char* nextWord = line_iterator_next_word(it);
	varData variablesData = { NULL };
	if (line_iterator_includes(it, OPEN_PAREN_CHAR)) {
		variablesData.label = line_iterator_next_until_parenthesis(it, OPEN_PAREN_CHAR);
		line_iterator_advance(it);//skips left parenthesis
		variablesData.leftVar = line_iterator_next_until_comma(it);
		line_iterator_advance(it);//skips comma
		variablesData.rightVar = line_iterator_next_until_parenthesis(it,CLOSE_PAREN_CHAR);
	}
	else {
		variablesData.label = line_iterator_next_word(it);
	}
	return variablesData;
}


