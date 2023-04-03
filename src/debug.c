#include "debug.h"
#include <string.h>

void print_error(char* start_pos, long line_num, errorCodes err_code)
{
	char err_buff[DEBUG_LINE_MAX_LENGTH] = { 0 };
	ptrdiff_t offset;

	/* Calculate the spacing between the start of the line and the error pos. */
	sprintf(err_buff, "Line %li:", line_num);

	printf("%s %s\n", err_buff, start_pos);
	printf("Error: %s\n\n", map_token_to_err(err_code));
}

char* map_token_to_err(errorCodes code)
{
	switch (code) {
	case ERROR_CODE_OK: return "No errors";
	case ERROR_CODE_UNKNOWN: return "Unknown error";
	case ERROR_CODE_INVALID_NAME: return "Invalid name";
	case ERROR_CODE_TO_MANY_OPERANDS: return "Too many operands";
	case ERROR_CODE_TO_LITTLE_OPERANDS: return "Too few operands";
	case ERROR_CODE_SYNTAX_ERROR: return "Syntax error";
	case ERROR_CODE_SYMBOL_REDEFINITION: return "Symbol redefinition";
	case ERROR_CODE_INVALID_SYM_TYPE: return "Invalid symbol type";
	case ERROR_CODE_SPACE_BEFORE_COLON: return "Space before colon";
	case ERROR_CODE_INVALID_CHAR_IN_LABEL: return "Invalid character in label";
	case ERROR_CODE_RESERVED_KEYWORD_DEF: return "Reserved keyword definition";
	case ERROR_CODE_SYMBOL_IGNORED_WARN: return "Symbol ignored (warning)";
	case ERROR_CODE_INVALID_AMOUNT_OF_OPERANDS: return "Invalid amount of operands";
	case ERROR_CODE_COMMA_AFTER_INSTRUCTION: return "Comma after instruction";
	case ERROR_CODE_MISSING_COMMA: return "Missing comma";
	case ERROR_CODE_VALUE_ERROR: return "Value error";
	case ERROR_CODE_EXTRA_COMMA: return "Extra comma";
	case ERROR_CODE_INVALID_LABEL_DEF: return "Invalid label definition";
	case ERROR_CODE_MISSING_OPEN_PAREN: return "Missing open parenthesis";
	case ERROR_CODE_INVALID_COMMA_POS: return "Invalid comma position";
	case ERROR_CODE_INVALID_INT: return "Invalid integer";
	case ERROR_CODE_SPACE_AFTER_OPERAND: return "Space after operand";
	case ERROR_CODE_INVALID_OPERAND: return "Invalid or couldn't find operand";
	case ERROR_CODE_INVALID_WHITE_SPACE: return "Invalid white space";
	case ERROR_CODE_EXTRA_PAREN: return "Extra parenthesis";
	case ERROR_CODE_MISSING_OPEN_QUOTES: return "Missing open quotes";
	case ERROR_CODE_MISSING_CLOSE_QUOTES: return "Missing close quotes";
	case ERROR_CODE_TEXT_AFTER_END: return "Text after end directive";
	case ERROR_CODE_MISSING_OPERAND: return "Missing operand";
	case ERROR_CODE_LABEL_DOES_NOT_EXISTS: return "Could not find Label in symbol map.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_EXTERN: return "Label already defined as extern.";
	case ERROR_CODE_LABEL_ALREADY_EXISTS_AS_ENTRY: return "Label already defined as entry.";
	case ERROR_CODE_LABEL_CANNOT_BE_DEFINED_AS_OPCODE_OR_REGISTER: return "Label cannot be defined as Opcode or Register";
	case ERROR_CODE_LABEL_MISSING_OR_NON_EXISTS_OPCODE: return "OPCODE does not exists or missing";
	default: return "Unknown error";
	}
}

