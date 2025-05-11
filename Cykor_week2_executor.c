#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "Cykor_week2_parser.h"
#include "Cykor_week2_internal_command.h"
#include "Cykor_week2_external_command.h"

#define MAX_TOKENS 120

char*builtin_list[] = {"cd", "pwd", "echo","exit"};
char*external_list[] = {"cat","ls","whoami", "grep", "wc", "cp","makdir","mv","rmdir","rm","find","sed","kill",
    "clear","ps","awk","sleep"};

//다음 chunk에 저장되어있는 연산자와 이전 명령의 수행 결과인 status 값을 바탕으로 다음 명령을 실행할지 결정하는 함수
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
    else if(operator == BACK){
        return 0;
    }
    else{
        return 0;
    }
}

//입력받은 문자열이 명령어에 해당하는지 검사하는 함수
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

//명령어 들어가는 인자 수 검사하는 함수
int check_argument_count(char** argv, int min, int max) {
    int count = 0;
    while (argv[count] != NULL) {
        count++;
    }
    if (count < min || count > max) {
        return 0;  // 인자 수 틀림
    }
    return 1;  // 인자 수 정상
}

//내장 함수의 실행에 대한 분기점을 만들어주는 함수
int execute_builtin_command(char**cmd_args){
        if(strcmp(cmd_args[0],"cd") == 0){
            if(!check_argument_count(cmd_args+1,0,1)){
                fprintf(stderr, "cd: too many arguments\n");
                return 0;
            }
            return execute_cd(cmd_args);
        }
        else if(strcmp(cmd_args[0],"pwd") == 0){
            if(!check_argument_count(cmd_args+1,0,0)){
                fprintf(stderr, "pwd: too many arguments\n");
                return 0;
            }
            return execute_pwd();
        }
        else if(strcmp(cmd_args[0], "exit") == 0){
            if(!check_argument_count(cmd_args+1,0,1)){
                fprintf(stderr, "exit: too many arguments\n");
                return 0;
            }
            return execute_exit();
        }
        else if(strcmp(cmd_args[0], "echo") == 0){
            return execute_echo(cmd_args);
        }
}

// 명령어 타입에 따라 외부 명령어 실행, 내장 명령어 실행, 에러 발생의 분기를 결정하는 함수
int execute_command(char ** token_array, TokenInfo *tokens, int is_background){

    //백그라운드 실행 시 출력을 위한 잡 번호호
    static int job_id = 1;

    // 첫번째 토큰의 명령어 타입에 따라 실행을 분기. 이때 실행 결과는 0 or 1의 형태로 status에 저장
    if (tokens[0].type == CMD_INTERNAL) {
        if (is_background) {
            pid_t pid = fork();
            if (pid == 0) {
                // 자식 프로세스: 내장 명령 실행 (의미는 없음)
                fflush(stdout);
                fflush(stderr);
                execute_builtin_command(token_array);
                exit(0);
            } else {
                // 부모는 job 번호 출력만 하고 프롬프트로
                printf("[%d] %d\n", job_id++, pid);
                fflush(stdout);
            }
            return 1;
    } else {
        return execute_builtin_command(token_array);  // 평소처럼 부모에서 실행
}
    } 
    else if(tokens[0].type == CMD_EXTERNAL)
    {
        return execute_external_command(token_array, is_background, &job_id);
    }
    else{
        fprintf(stderr, "%s: command not found\n",token_array[0]);
        return 0;
    }

}

//파이프 라인을 처리하는 함수
int execute_pipe(char** cmd_args, ChunkInfo *pipe_chunk, int pipe_count, int is_background){

    int cmd_count = pipe_count +1;
    int pipes[cmd_count - 1][2];  // 2개의 파이프 필요
    int pipe_token_count = 0;
    pid_t pids[cmd_count];
    char**pipe_token_array;
    TokenInfo pipe_tokens[MAX_TOKENS];
    int status = 0;

    if(pipe_chunk[0].type == PIPE){
        status = 0;
        fprintf(stderr, "bash: syntax error near unexpected token \'|\'\n");
        return 0;
    }

    // 파이프 생성
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmd_count; i++) {
        int k = 2*i;
        pipe_token_count = 0; 
        scan_tokens(cmd_args[k], pipe_tokens, &pipe_token_count);
        pipe_token_array = build_token_array(cmd_args[k], pipe_tokens, pipe_token_count);
        is_command(pipe_token_array[0], pipe_tokens);
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            // 자식 프로세스

            // 입력 연결 (i > 0이면 앞 파이프의 read를 stdin에 연결)
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // 출력 연결 (i < NUM_CMDS - 1이면 뒷 파이프의 write를 stdout에 연결)
            if (i < cmd_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // 파이프 전부 닫기 (중복 close 방지)
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // 명령 실행
            status = execute_command(pipe_token_array, pipe_tokens, is_background);
            exit(status);
        }
    }

    // 부모는 모든 파이프 닫고 자식 기다림
    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
    free_token_array(pipe_token_array);
    return status;
}

//전체 명령어 실행에 대한 중앙 분기점
int command_branch(char **command_chunk, ChunkInfo *chunk, int chunk_count, int chunk_num) {
    //몇 번째 청크를 대상으로 명령 수행을 시작할 것인지 구분하기 위한 장치
    int i = chunk_num;
    int status = 0;
    int token_count = 0;
    char**token_array;
    int is_background = 0;
    TokenInfo tokens[MAX_TOKENS];

    // &&나 ||가 앞에 나오는 경우를 잡아내서 에러를 발생시키는 코드
    if(chunk[i].type != CHUNK){
        status = 0;
        fprintf(stderr, "bash: syntax error near unexpected token \'%s\'\n",command_chunk[i]);
        return 0;
    }
    if(chunk[i+1].type == BACK){
        is_background = 1;
    }


    //청크를 토큰화 시키고 배열로 만들어 처리에 용이하게 가공 - 만약 파이프가 감지되면 파이프 실행으로 분기
    if(scan_tokens(command_chunk[i], tokens, &token_count)){

        int command_count = 0;
        int pipe_count = 0;
        char**pipe_array;
        ChunkInfo pipe_chunk[120];

        scan_pipe(command_chunk[i], pipe_chunk, &command_count, &pipe_count);
        pipe_array = build_chunk_array(command_chunk[i], pipe_chunk, command_count);
        status = execute_pipe(pipe_array, pipe_chunk, pipe_count, is_background);
        free_token_array(pipe_array);

    }
    else{
        token_array = build_token_array(command_chunk[i], tokens, token_count);
        //청크의 첫번째 토큰을 검사해서 명령어인지 아닌지 검사
        is_command(token_array[0], tokens);

        status = execute_command(token_array, tokens, is_background);
        //토큰들 동적할당 해뒀던 메모리 해제
        free_token_array(token_array);
    }
    
    //다음 청크의 실행을 결정하는 부분. chunk 배열의 마지막에는 '\0'이 있으므로 chunk[i+1]을 호출해도 Segmentation Fault는 발생하지 않는다!
    if(should_execute_next(status, chunk[i+1].type)){
        chunk_num += 2;
        command_branch(command_chunk, chunk, chunk_count, chunk_num);
    }

    return status;
}


