#ifndef PARSER_H
#define PARSER_H

typedef enum {
    CMD_NONE,
    CMD_INTERNAL,
    CMD_EXTERNAL,
    AND,
    OR,
    BACK,
    PIPE
} TokenType;


typedef struct {
    TokenType type; // "PIPE", "OR", "COMMAND", etc
    int start;
    int end;
} TokenInfo;



int scan_tokens(const char *input, TokenInfo *tokens, int *token_count);
int parse_with_token_info(const char *input, TokenInfo *tokens, int token_count);
void free_token_types(TokenInfo *tokens, int token_count);
char **build_token_array(const char *input, TokenInfo *tokens, int token_count);
void free_token_array(char **array);

#endif
