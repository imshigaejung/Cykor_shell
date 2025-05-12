#ifndef PARSER_H
#define PARSER_H

//초기에는 문자열로 타입을 지정했으나, 안정성을 위해 열거형으로 타입 지정 방식 교체
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

//입력을 스캔하여 인덱싱 정보를 담기 위한 자료형
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


int scan_tokens(const char*chunk, TokenInfo *tokens, int* token_count);
int scan_chunk(const char *input, ChunkInfo *chunk, int *chunk_count);
int scan_pipe(const char *input, ChunkInfo *chunk, int *chunk_count, int *pipe_count);
char **build_token_array(const char *input, TokenInfo *tokens, int token_count);
char **build_chunk_array(const char *input, ChunkInfo*chunk, int chunk_count);
void free_token_array(char **array);

#endif
