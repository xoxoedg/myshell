#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

typedef enum {
    TOKEN_WORD, // ls, -la, "a | b"
    TOKEN_PIPE, // |
    TOKEN_AND, // &&
    TOKEN_OR, // ||
    TOKEN_SEMICOLON, // ;
    TOKEN_BACKGROUND, // &
    TOKEN_REDIRECT_IN, //
    TOKEN_REDIRECT_OUT, // >
    TOKEN_REDIRECT_APPEND, // >>
    TOKEN_EOF // Ende der Liste
} TokenType;

typedef struct {
    TokenType type;
    char *text; 
} Token;

typedef struct {
    Token *items; 
    size_t count; 
    size_t capacity; 
} TokenList;

static bool isOperator(char c) {
    return c == '>' || c == '<' || c == '&' || c == '|' || c == ';';
}

static bool isWordChar(char c) {
    return c != '\0' && !isspace((unsigned char)c) && !isOperator(c);
}

static void token_list_push(TokenList *list, TokenType type, char *text) {
    if (list->count == list->capacity) { }
    list->items[list->count].type = type;
    list->items[list->count].text = text;
    list->count++;
}

TokenList *tokenize(const char *input) {
    TokenList *token_list = malloc(sizeof(TokenList));

    if (token_list == NULL) {
        return NULL;
    }

    token_list->count = 0;
    token_list->capacity = 4;

    token_list->items = malloc(token_list->capacity * sizeof(Token));
    if (token_list->items == NULL) {
        free(token_list);
        return NULL;
    }

    size_t i = 0;
    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
        } else {
            size_t start = i;
            while (isWordChar(input[i])) { 
                i++;
            }
            size_t length = i-start;
            char* wordChar = malloc(length + 1);
            memcpy(wordChar, input + start, length);
            wordChar[length] = '\0';
            token_list_push(token_list, TOKEN_WORD, wordChar); 
        }
    }
    token_list_push(token_list, TOKEN_EOF, NULL);
    return token_list;
}




