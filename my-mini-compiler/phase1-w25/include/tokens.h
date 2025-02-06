//
// Created by Youssef
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

#define MAX_NUMBER_SIZE 32767
#define MIN_NUMBER_SIZE -32768

/* Token types that need to be recognized by the lexer
 * TODO: Add more token types as per requirements:
 * - Keywords or reserved words (if, repeat, until)
 * - Identifiers
 * - String literals
 * - More operators
 * - Delimiters
 */
typedef enum {
  TOKEN_EOF,
  TOKEN_NUMBER,     // e.g., "123", "456"
  TOKEN_OPERATOR,   // e.g., "+", "-"
  TOKEN_KEYWORD,    // e.g., "if", "until"
  TOKEN_IDENTIFIER, // e.g., "x"
  TOKEN_STRING,     // e.g., ""Hello World""
  TOKEN_DELIMITER,  // e.g., "()", "{}"
  TOKEN_COMMENT,    // e.g., "// comment", "/* comment */"
  TOKEN_ERROR
} TokenType;

/* Error types for lexical analysis
 * TODO: Add more error types as needed for your language - as much as you like
 * !!
 */
typedef enum {
  ERROR_NONE,
  ERROR_INVALID_CHAR,
  ERROR_INVALID_NUMBER_VALUE,
  ERROR_INVALID_NUMBER_FORMAT,
  ERROR_CONSECUTIVE_OPERATORS,
  ERROR_UNTERMINATED_STRING,
  ERROR_UNEXPECTED_CHARACTER,
  ERROR_IDENTIFIER_TOO_LONG
} ErrorType;

/* Token structure to store token information
 * TODO: Add more fields if needed for your implementation
 * Hint: You might want to consider adding line and column tracking if you want
 * to debug your lexer properly. Don't forget to update the token fields in
 * lexer.c as well
 */
typedef struct {
  TokenType type;
  char lexeme[100]; // Actual text of the token
  int line;         // Line number in source file
  ErrorType error;  // Error type if any
} Token;

#endif /* TOKENS_H */
