#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "../include/raw_file.h"

#define TOKEN_ARRAY_INIT_CAPACITY 1024

typedef enum TOKEN_TYPE {
    // Single character tokens
    TT_LEFT_CURLY,
    TT_RIGHT_CURLY,
    TT_LEFT_SQUARE,
    TT_RIGHT_SQUARE,
    TT_COLON,
    TT_COMMA,
    // Multi-character tokens
    TT_STRING,
    TT_NUMBER,
    TT_TRUE,
    TT_FALSE,
    TT_NULL,
} TokenType;

typedef struct TOKEN_ITEM {
    TokenType type;
    char *value;
} TokenItem;

typedef struct TOKEN_ARRAY {
    size_t count;
    size_t capacity;
    TokenItem items[];
} TokenArray;

TokenItem *token_item_create(TokenItem *prev, TokenType type, char *value) {
    TokenItem *ti = malloc(sizeof(TokenItem));
    ti->type = type;
    return ti;
}

TokenArray *token_array_create(void) {
    TokenArray *tokens = NULL;
    tokens = malloc(sizeof(TokenArray) + (sizeof(TokenItem) * TOKEN_ARRAY_INIT_CAPACITY));
    if (tokens == NULL) {
        return NULL;
    }
    tokens->count = 0;
    tokens->capacity = TOKEN_ARRAY_INIT_CAPACITY;
    return tokens;
}

TokenItem *add_token(TokenArray *tokens, TokenType tt, char *buf, size_t start, size_t cur) {
    if (tokens->count >= tokens->capacity) {
        return NULL;
    }
    tokens->count++;
    TokenItem *ti = &tokens->items[tokens->count];
    ti->type = tt;
    size_t length = cur - start;
    ti->value = malloc(length + 1);
    strncpy(ti->value, buf, length + 1);
    return ti;
}

int raw_file_scan(RawFile *raw_file, TokenArray *tokens) {
    tokens->count = 0;
    tokens->capacity = 1024;

    size_t start = 0;
    for (int c = 0; c < raw_file->file_size; c++) {
        start = c;
        switch (raw_file->buf[c]) {
            case '{': add_token(tokens, TT_LEFT_CURLY, raw_file->buf, start, c); break;
            case '}': add_token(tokens, TT_RIGHT_CURLY, raw_file->buf, start, c); break;
            case '[': add_token(tokens, TT_LEFT_SQUARE, raw_file->buf, start, c); break;
            case ']': add_token(tokens, TT_RIGHT_SQUARE, raw_file->buf, start, c); break;
            case ',': add_token(tokens, TT_COMMA, raw_file->buf, start, c); break;
            case ':': add_token(tokens, TT_COLON, raw_file->buf, start, c); break;
            case '"':
                while( raw_file->buf[++c] != '"') {
                    ;
                }
                add_token(tokens, TT_STRING, raw_file->buf, start, c);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
                while (raw_file->buf[c] != ',' && raw_file->buf[c] != ' ') {
                    c++;
                }
                add_token(tokens, TT_NUMBER, raw_file->buf, start, c);
                c--;
                break;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    RawFile *raw_file = raw_file_create(argv[1]);

    if (raw_file_read(raw_file) < 0) {
        printf("Error reading file %s\n", raw_file->file_name);
        return -1;
    }

    TokenArray *tokens = token_array_create();
    if (tokens == NULL) {
        return -1;
    }
    raw_file_scan(raw_file, tokens);

    for(size_t s = 0; s < tokens->count; s++) {
        TokenItem *token = &tokens->items[s];

        switch (token->type) {
            case TT_LEFT_SQUARE:
                printf("[");
                break;
            case TT_RIGHT_SQUARE:
                printf("]");
                break;
            case TT_LEFT_CURLY:
                printf("{");
                break;
            case TT_RIGHT_CURLY:
                printf("}");
                break;
            case TT_COMMA:
                printf(",");
                break;
            case TT_COLON:
                printf(":");
                break;
            case TT_STRING:
            case TT_NUMBER:
                printf("%s", token->value);
                break;
        }
    }

    raw_file_destroy(raw_file);

    return 0;
}
