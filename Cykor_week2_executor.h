#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "Cykor_week2_parser.h"



int is_command(const char*command, TokenInfo*tokens);
int execute_command(char **command_chunk, ChunkInfo *chunk, int chunk_count, int chunk_num);
int execute_builtin_command(char**cmd_args);
int should_execute_next(int status, ChunkType operator);



#endif
