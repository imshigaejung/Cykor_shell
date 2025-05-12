#ifndef PARSER_H
#define PARSER_H

typedef enum {
    CHUNK,
    AND,
    OR,
    BACK,
    PIPE,
    SEPERATOR
} ChunkType;

typedef enum {
    CMD_INTERNAL,
    CMD_EXTERNAL,
    PARAM
} TokenType;

typedef struct {
    ChunkType type;
    int start;
    int end;
} ChunkInfo;

typedef struct 
{
    TokenType type;
    int start;
    int end;
}TokenInfo;



int scan_tokens(const char*chunk, TokenInfo **chunk_ptr, int* token_count);
int scan_chunk(const char *input, ChunkInfo **chunk_ptr, int *chunk_count);
int scan_pipe(const char *input, ChunkInfo **chunk_ptr, int *chunk_count, int *pipe_count);
char **build_token_array(const char *input, TokenInfo *tokens, int token_count);
char **build_chunk_array(const char *input, ChunkInfo*chunk, int chunk_count);
void free_token_array(char **array);

#endif
