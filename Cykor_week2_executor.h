#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "Cykor_week2_parser.h"



int scan_command(char**command_array, TokenInfo *tokens, int token_count);
TokenType is_command(const char*command);
void execute_command(char**command_array, TokenInfo *tokens, int token_count);
void execute_builtin(char**cmd_args);
void execute_external(char**cmd_args);



#endif