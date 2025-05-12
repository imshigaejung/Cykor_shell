#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "Cykor_week2_parser.h"



int is_command(const char*command, TokenInfo*tokens);
int command_branch(char **command_chunk, ChunkInfo *chunk, int chunk_count, int chunk_num);
int execute_command(char ** token_array, TokenInfo *tokens, int is_background);
int execute_builtin_command(char**cmd_args);
int should_execute_next(int status, ChunkType operator);
int execute_pipe(char** cmd_args, ChunkInfo *pipe_chunk, int pipe_count, int is_background);
const char* token_type_to_str(ChunkType type); 

#endif
