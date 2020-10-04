#ifndef IFJ20C_SCANNER_H
#define IFJ20C_SCANNER_H

#include "token.h"
#include "stdio.h"

/**
 * Sets file stream for lexical analysis
 * @param[IN] file
 * @return -1 when file is NULL
 */
int set_file(FILE* file);

/**
 * Lexes next token from open file
 * @param[OUT] token
 * @return -1 on error, 1 on success, 0 on EOF
 */
int next_token(Token* token);

/**
 * @param[IN] attribute
 * @returns appropriate Keyword type for keyword attribute. Returns KEYWORD_NOT_A_KEYWORD if attribute is not a keyword
 */
Keyword get_keyword(const char* attribute);
#endif //IFJ20C_SCANNER_H
