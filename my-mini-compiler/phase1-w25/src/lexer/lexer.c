/* lexer.c */
#include "../../include/tokens.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Line tracking
static int current_line = 1;
static char last_token_type = 'x'; // For checking consecutive operators

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
  printf("Lexical Error at line %d: ", line);
  switch (error) {
  case ERROR_INVALID_CHAR:
    printf("Invalid character '%s'\n", lexeme);
    break;
  case ERROR_INVALID_NUMBER:
    printf("Invalid number format\n");
    break;
  case ERROR_CONSECUTIVE_OPERATORS:
    printf("Consecutive operators not allowed\n");
    break;
  default:
    printf("Unknown error\n");
  }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

void print_token(Token token) {
  if (token.error != ERROR_NONE) {
    print_error(token.error, token.line, token.lexeme);
    return;
  }

  printf("Token: ");
  switch (token.type) {
  case TOKEN_NUMBER:
    printf("NUMBER");
    break;
  case TOKEN_OPERATOR:
    printf("OPERATOR");
    break;
  case TOKEN_EOF:
    printf("EOF");
    break;
  default:
    printf("UNKNOWN");
  }
  printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}

/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
  Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
  char c;

  // Skip whitespace and track line numbers
  while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
    if (c == '\n') {
      current_line++;
    }
    (*pos)++;
  }

  if (input[*pos] == '\0') {
    token.type = TOKEN_EOF;
    strcpy(token.lexeme, "EOF");
    return token;
  }

  c = input[*pos];

  // TODO: Add comment handling here

  // Handle numbers
  if (isdigit(c)) {
    int i = 0;
    do {
      token.lexeme[i++] = c;
      (*pos)++;
      c = input[*pos];
    } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

    token.lexeme[i] = '\0';
    token.type = TOKEN_NUMBER;
    return token;
  }

  // TODO: Add keyword and identifier handling here
  // Hint: You'll have to add support for keywords and identifiers, and then
  // string literals

  // TODO: Add string literal handling here

  // Handle operators
  if (c == '+' || c == '-') {
    if (last_token_type == 'o') {
      // Check for consecutive operators
      token.error = ERROR_CONSECUTIVE_OPERATORS;
      token.lexeme[0] = c;
      token.lexeme[1] = '\0';
      (*pos)++;
      return token;
    }
    token.type = TOKEN_OPERATOR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    last_token_type = 'o';
    (*pos)++;
    return token;
  }

  // TODO: Add delimiter handling here

  // Handle invalid characters
  token.error = ERROR_INVALID_CHAR;
  token.lexeme[0] = c;
  token.lexeme[1] = '\0';
  (*pos)++;
  return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic
// operators (+ and -), consecutive operator errors, whitespace and newlines,
// with simple line tracking for error reporting.

int main() {
  const char *input = "123 + 456 - 789\n1 ++ 2"; // Test with multi-line input
  int position = 0;
  Token token;

  printf("Analyzing input:\n%s\n\n", input);

  do {
    token = get_next_token(input, &position);
    print_token(token);
  } while (token.type != TOKEN_EOF);

  return 0;
}
