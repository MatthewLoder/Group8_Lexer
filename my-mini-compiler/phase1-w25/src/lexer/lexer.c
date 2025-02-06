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
  case ERROR_INVALID_NUMBER_FORMAT:
    printf("Invalid number format\n");
    break;
  case ERROR_INVALID_NUMBER_VALUE:
    printf("Invalid number value (needs to be within %d to %d)\n", MIN_NUMBER_SIZE, MAX_NUMBER_SIZE);
    break;
  case ERROR_CONSECUTIVE_OPERATORS:
    printf("Consecutive operators not allowed\n");
    break;
  case ERROR_UNTERMINATED_STRING:
    printf("Unterminated string literal\n");
    break;
  case ERROR_IDENTIFIER_TOO_LONG:
    printf("Identifier name too long\n");
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

  // create end of file token
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
    int i = 0;
    do {
      token.lexeme[i++] = c;
      (*pos)++;
      c = input[*pos];
    } while (c != '\n' && i < sizeof(token.lexeme) - 1);

    token.lexeme[i] = '\0';
    token.type = TOKEN_COMMENT;
    last_token_type = 'c';
    return token;
  }

  // Multi-Line Comments
  if (c == '/' && input[*pos + 1] == '*') {
    int i = 0;

    token.lexeme[i++] = c;
    (*pos)++;
    c = input[*pos];
    token.lexeme[i++] = c;
    (*pos)++;

    do {
      c = input[*pos];

      if (i < sizeof(token.lexeme) - 1) {
        token.lexeme[i++] = c;
      }

      (*pos)++;

    } while (!((c == '*' && input[*pos] == '/')) && *pos < strlen(input) - 1);

    if (i < sizeof(token.lexeme) - 1) {
      token.lexeme[i++] = input[*pos];
    }
    (*pos)++;

    if (i < sizeof(token.lexeme) - 1) {
      token.lexeme[i++] = input[*pos];
    }
    (*pos)++;

    token.lexeme[i] = '\0';

    token.type = TOKEN_COMMENT;
    last_token_type = 'c';

    return token;
  }

  // Handle numbers
  if (isdigit(c) || c == '-') {
    int i = 0;
    int isOperator = 0;

    //check if number hyphen is an operator or part of a number
    if (c == '-') {
      token.lexeme[i + 1] = c;
      if (!isdigit(input[(*pos) + 1])) isOperator = 1;
    }

    if (!isOperator) {
      do {
        token.lexeme[i++] = c;
        (*pos)++;
        c = input[*pos];
        
        //check for invalid character in number
        if (!isdigit(c) && c != '\n' && c != ';' && c != '\t'  && c != ' ' && c != '\0' && c != '+' && c != '-' && c != '*' && c != '/' && c != '&' && c != '|' && c != '%' && c != '=' && c != ')' && c != '(' && c != '}' && c != '{') token.error = ERROR_INVALID_NUMBER_FORMAT;

      } while (((token.error == ERROR_NONE && isdigit(c)) || (token.error == ERROR_INVALID_NUMBER_FORMAT && c != '\n' && c != ';' && c != '\t' && c != ' ' && c != '+' && c != '-' && c != '*' && c != '/' && c != '&' && c != '|' && c != '%' && c != '=' && c != ')' && c != '(' && c != '}' && c != '{')) && i < sizeof(token.lexeme) - 1);

      token.lexeme[i] = '\0';

      if (token.error == ERROR_NONE) {
        int number = atoi(token.lexeme);

        if (number < MIN_NUMBER_SIZE || number > MAX_NUMBER_SIZE) {
          token.error = ERROR_INVALID_NUMBER_VALUE;
        }

      }
      
      token.type = TOKEN_NUMBER;
      last_token_type = 'n';
      return token;
    }
  }

  // TODO: Add keyword and identifier handling here
  // check if starts with a letter or underscore
  if (isalpha(c) || c == '_') {
    int i = 0;
    do {
      token.lexeme[i++] = c;
      (*pos)++;
      c = input[*pos];
    } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1); // keep going as long as we're still
                                            // finding letters or underscores

    if (i == sizeof(token.lexeme) - 1) {
      token.error = ERROR_IDENTIFIER_TOO_LONG;
    }

    token.lexeme[i] = '\0';

    // identify token as keyword or identifier
    if (strcmp(token.lexeme, "if") == 0 ||
        strcmp(token.lexeme, "repeat") == 0 ||
        strcmp(token.lexeme, "until") == 0 ||
        strcmp(token.lexeme, "else") == 0 ||
        strcmp(token.lexeme, "while") == 0 ||
        strcmp(token.lexeme, "for") == 0 || 
        strcmp(token.lexeme, "do") == 0 ||
        strcmp(token.lexeme, "return") == 0 ||
        strcmp(token.lexeme, "int\0") == 0) {
      token.type = TOKEN_KEYWORD;
      last_token_type = 'k';
    } else {
      token.type = TOKEN_IDENTIFIER;
      last_token_type = 'i';
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
      
      //adjust for extra quotation, max string length is 98 + 2 quotations
      if (i > sizeof(token.lexeme) - 2 || c == '\0') {
        token.error = ERROR_UNTERMINATED_STRING;
        *(pos)--;
        break;
      }
    } while (c != '\"');

    if (c == '\"') {
      token.lexeme[i++] = c;
    }
    
    (*pos)++;
    c = input[*pos];
    token.lexeme[i] = '\0';
    token.type = TOKEN_STRING;
    last_token_type = 's';
    return token;
  }

  // Handle operators
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' ||
      c == '%' || c == '=' || c == '!') {
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

  if (c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == '(' ||
      c == ')' || c == ';') {
    token.lexeme[0] = c;
    (*pos)++;
    c = input[*pos];
    token.type = TOKEN_DELIMITER;
    last_token_type = 'd';
    return token;
  }

  // Handle invalid characters
  token.error = ERROR_INVALID_CHAR;
  token.lexeme[0] = c;
  token.lexeme[1] = '\0';
  (*pos)++;
  return token;
}

void print_raw(const char *buffer) {
  while (*buffer) {
    switch (*buffer) {
    case '\n':
      printf("\\n");
      break;
    case '\t':
      printf("\\t");
      break;
    case '\r':
      printf("\\r");
      break;
    case '\0':
      printf("\\0");
      break;
    default:
      putchar(*buffer);
    }
    buffer++;
  }
  printf("\n\n");
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

  // get file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  // get buffer sizes
  char *buffer = malloc(file_size + 1);
  if (!buffer) {
    printf("Memory allocation failed.\n");
    fclose(file);
    return 1;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  buffer[bytes_read] = '\0';

  //drop and ignore carriage return (on windows)
  size_t j = 0;
  for (size_t i = 0; i < bytes_read; i++) {
      if (buffer[i] != '\r') {
          buffer[j++] = buffer[i];
      }
  }
  buffer[j] = '\0';

  print_raw(buffer);
  
  int position = 0;
  Token token;

  printf("Analyzing input:\n%s\n\n", buffer);

  int i = 0;

  do {
    token = get_next_token(buffer, &position);
    print_token(token);
    //printf("%d\n", i++);
  } while (token.type != TOKEN_EOF);

fclose(file);
  return 0;
} 