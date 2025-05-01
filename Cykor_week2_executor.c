#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "Cykor_week2_executor.h"
#include "Cykor_week2_parser.h"
#include "Cykor_week2_internal_command.h"
#include "Cykor_week2_external_command.h"


char*builtin_list[] = {"cd", "pwd", "echo","exit","clear"};
char*external_list[] = {"cat","ls","whoami"};


TokenType is_command(const char*command){

    int builtin_len = sizeof(builtin_list)/sizeof(char*);
    int external_len = sizeof(external_list)/sizeof(char*);

    for(int i = 0; i<builtin_len; i++){
        if (strcmp(command, builtin_list[i]) == 0) {
            return CMD_INTERNAL;
        }
    }
    for(int i = 0; i<external_len; i++){
        if(strcmp(command, external_list[i]) == 0) {
            return CMD_EXTERNAL;
        }

    }
    return CMD_NONE;
}

int scan_command(char**command_array, TokenInfo *tokens, int token_count){
    if ((command_array[0] == NULL) || (tokens == NULL))
        return -1;

    for(int i = 0; i<token_count; i++){

        if(tokens[i].type != CMD_NONE){
            continue;
        }
        else{
            tokens[i].type = is_command(command_array[i]);
        }
    } 
    return 0;
}

void execute_builtin(char**cmd_args){

        if(strcmp(cmd_args[0],"cd") == 0){
            execute_cd(cmd_args);
        }
        else if(strcmp(cmd_args[0],"pwd") == 0){
            execute_pwd();
        }
        else if(strcmp(cmd_args[0], "exit") == 0){
            execute_exit();
        }
}

void execute_external(char**cmd_args){

}

void execute_command(char **command_array, TokenInfo *tokens, int token_count) {
    int i = 0;

    while (i < token_count) {
        // 현재 토큰이 명령어면 실행 준비
        if (tokens[i].type == CMD_INTERNAL || tokens[i].type == CMD_EXTERNAL) {
            int cmd_start = i;
            int cmd_end = i;

            // 인자는 CMD_NONE인 것만 포함
            for (int j = i + 1; j < token_count; j++) {
                if (tokens[j].type == AND || tokens[j].type == OR || tokens[j].type == PIPE || tokens[j].type == BACK) {
                    break;
                }
                
                cmd_end = j;
            }

            // 명령어 + 인자 배열 만들기
            int argc = cmd_end - cmd_start + 2; // +1 for NULL
            char *cmd_args[argc];
            for (int k = cmd_start; k <= cmd_end; k++) {
                cmd_args[k - cmd_start] = command_array[k];
            }
            cmd_args[argc - 1] = NULL;

            // 실행
            if (tokens[i].type == CMD_INTERNAL) {
                execute_builtin(cmd_args);
            } else {
                execute_external(cmd_args);
            }

            i = cmd_end + 1; // 다음 명령어로 넘어감
        } else {
            i++; // 명령어가 아니면 다음 토큰으로
        }
    }
}


