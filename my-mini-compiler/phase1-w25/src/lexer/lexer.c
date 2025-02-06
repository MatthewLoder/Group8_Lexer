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
  case ERROR_UNTERMINATED_STRING:
    printf("Unterminated string literal\n");
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
  case TOKEN_KEYWORD:
    printf("KEYWORD");
    break;
  case TOKEN_IDENTIFIER:
    printf("IDENTIFIER");
    break;
  case TOKEN_STRING:
    printf("STRING");
    break;
  case TOKEN_DELIMITER:
    printf("DELIMITER");
    break;
  case TOKEN_COMMENT:
    printf("COMMENT");
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

  // create eof token
  if (input[*pos] == '\0') {
    token.type = TOKEN_EOF;
    strcpy(token.lexeme, "EOF");
    return token;
  }

  c = input[*pos];

  // TODO: Add comment handling here
  // Single-Line Comments
  if (c == '/' &&
      input[*pos + 1] == '/') // check if the first 2 characters are //
  {
    while (input[*pos] != '\n' &&
           input[*pos] != '\0') // until we find a new line character,
    {
      (*pos)++; // keep skipping contents
    }
    last_token_type = 'C'; // Mark as comment
    return get_next_token(input, pos);
  }

  // Multi-Line Comments
  if (c == '/' && input[*pos + 1] == '*') // check ig first 2 characters are /*
  {
    (*pos) += 2;
    while (input[*pos] != '\0' &&
           !(input[*pos] == '*' &&
             input[*pos + 1] == '/')) // until we find the closing */
    {
      if (input[*pos] == '\n')
        current_line++; // keep skipping contents
      (*pos)++;
    }
    if (input[*pos] == '*' && input[*pos + 1] == '/')
      (*pos) += 2;
    last_token_type = 'C'; // Mark as comment
    return get_next_token(input, pos);
  }

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
  // check if starts with a letter or underscore
  if (isalpha(c) || c == '_') {
    int i = 0;
    do {
      token.lexeme[i++] = c;
      (*pos)++;
      c = input[*pos];
    } while ((isalnum(c) || c == '_') &&
             i < sizeof(token.lexeme) - 1); // keep going as long as we're still
                                            // finding letters or underscores

    token.lexeme[i] = '\0';

    // identify token as keyword or identifier
    if (strcmp(token.lexeme, "if") == 0 ||
        strcmp(token.lexeme, "repeat") == 0 ||
        strcmp(token.lexeme, "until") == 0 ||
        strcmp(token.lexeme, "else") == 0 ||
        strcmp(token.lexeme, "while") == 0 ||
        strcmp(token.lexeme, "for") == 0 || strcmp(token.lexeme, "do") == 0 ||
        strcmp(token.lexeme, "return") == 0 ||
        strcmp(token.lexeme, "int") == 0) {
      token.type = TOKEN_KEYWORD;
    } else {
      token.type = TOKEN_IDENTIFIER;
    }

    return token;
  }

  // TODO: Add string literal handling here
  if (c == '\"') {
    int i = 0;

    do {
      token.lexeme[i++] = c;
      (*pos)++;
      c = input[*pos];
    } while (c != '\"');

    token.lexeme[i++] = c;
    (*pos)++;
    c = input[*pos];

    token.lexeme[i] = '\0';

    token.type = TOKEN_STRING;
    return token;
  }

  // Handle operators
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '%') {
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

  if (c == '{' || c == '}' || c == '[' || c == ']' || c == ','|| c == '(' || c == ')' || c == ';') {
    token.lexeme[0] = c;
    (*pos)++;
    c = input[*pos];
    token.type = TOKEN_DELIMITER;
    return token;
  }

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
  // const char *input = "123 + 456 - 789\n1 ++ 2"; // Test with multi-line
  // input

  // Test comments, keywords, identifiers
  // const char *input =
  //"// This is a comment \n /* Multi-line \n comment */ int x";

  FILE *file = fopen("../../test/input_invalid.txt", "r");
  if (file == NULL) {
        printf("Error opening file\n");
        return 1; 
    }
  printf("test");
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    int position = 0;
    Token token;
    
    printf("Analyzing input:\n%s\n\n", buffer);

    int i = 0;

    do {
      token = get_next_token(buffer, &position);
      print_token(token);
      printf("%d\n", i++);
      // return 0;
    } while (token.type != TOKEN_EOF);
  }

fclose(file);

  return 0;
}
