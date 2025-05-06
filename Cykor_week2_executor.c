#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "Cykor_week2_executor.h"
#include "Cykor_week2_parser.h"
#include "Cykor_week2_internal_command.h"
#include "Cykor_week2_external_command.h"

#define MAX_TOKENS 120

char*builtin_list[] = {"cd", "pwd", "echo","exit","clear"};
char*external_list[] = {"cat","ls","whoami"};

int should_execute_next(int status, ChunkType operator){

    if(status == 0 && operator == OR){
        return 1;
    }
    else if(status == 1 && operator == AND){
        return 1;
    }
    else if(operator == SEPERATOR){
        return 1;
    }
    else{
        return 0;
    }
}

int is_command(const char*command, TokenInfo*tokens){

    int builtin_len = sizeof(builtin_list)/sizeof(char*);
    int external_len = sizeof(external_list)/sizeof(char*);

    for(int i =0; i<builtin_len; i++){
        if (strcmp(command, builtin_list[i]) == 0) {
            tokens[0].type = CMD_INTERNAL;
        }
    }
    for(int i = 0; i< external_len; i++){
        if(strcmp(command, external_list[i]) == 0) {
            tokens[0].type = CMD_EXTERNAL;
        }
    }
    return 0;
}

int execute_builtin_command(char**cmd_args){

        if(strcmp(cmd_args[0],"cd") == 0){
            return execute_cd(cmd_args);
        }
        else if(strcmp(cmd_args[0],"pwd") == 0){
            return execute_pwd();
        }
        else if(strcmp(cmd_args[0], "exit") == 0){
            return execute_exit();
        }
}


int execute_command(char **command_chunk, ChunkInfo *chunk, int chunk_count, int chunk_num) {
    int i = chunk_num;
    int status = 0;
    int token_count = 0;
    char**token_array;
    TokenInfo tokens[MAX_TOKENS];

    scan_tokens(command_chunk[i], tokens, &token_count);
    token_array = build_token_array(command_chunk[i], tokens, token_count);
    is_command(token_array[0], tokens);

      // 실행
    if (tokens[0].type == CMD_INTERNAL) {
        status = execute_builtin_command(token_array);
    } 
    else if(tokens[0].type == CMD_EXTERNAL)
    {
        status = execute_external_command(token_array);
    }
    else{
        status = 0;
        fprintf(stderr, "%s: command not found\n",token_array[0]);
    }

    if(should_execute_next(status, chunk[i+1].type)){
        chunk_num += 2;
        execute_command(command_chunk, chunk, chunk_count, chunk_num);
    }
}
