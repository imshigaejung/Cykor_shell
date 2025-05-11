#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//cd 입력 시 디렉토리 이동을 구현하는 함수
int execute_cd(char **cmd_args) {
    // 디렉토리 동적으로 입력받음 - 중간에 끊김 방지지
    char *target_path = NULL;

    // cd 이후 입력이 없는 경우우
    if (cmd_args[1] == NULL) {
        char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            return 0;
        }
        target_path = home;  // HOME으로 이동
    } 
    // ~로 HOME을 입력하는 경우
    else if (cmd_args[1][0] == '~') {
        char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            return 0;
        }
        // 홈 디렉토리와 경로 뒷부분을 붙이기 위한 동적 할당
        const char *directory = cmd_args[1] + 1;
        size_t len = strlen(home) + strlen(directory) + 1;
        target_path = malloc(len);
        if (!target_path) {
            perror("malloc");
            return 0;
        }

        // 홈 디렉토리 + 경로 뒷부분을 한 줄로 붙임
        snprintf(target_path, len, "%s%s", home, directory);
    } 
    // 일반적인 경우
    else {
        target_path = cmd_args[1];  
    }

    //디렉토리 변경, 실패한 경우 
    if (chdir(target_path) != 0) {
        perror("bash: cd");

        //~로 입력한 경우 아까 전에 추가로 malloc을 사용했던 target_path의 메모리 해제
        if (cmd_args[1] && cmd_args[1][0] == '~') free(target_path);
        return 0;
    }
    //윗 부분과 마찬가지로 메모리 해제
    if (cmd_args[1] && cmd_args[1][0] == '~') free(target_path);
    return 1;
}

//pwd 입력 시 현재 디렉토리를 출력하는 함수
int execute_pwd(){
    char*cwd = getcwd(NULL,0);

    if (cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd);  // 동적 메모리 해제
        return 1;
    } else {
        perror("pwd");
        return 0;
    }
}

//exit 입력 시 프로그램을 종료하는 함수
int execute_exit(){
    exit(0);
}

//echo 입력 시 echo 뒤에 받은 입력을 다시 출력하는 함수
int execute_echo(char **cmd_args) {
    int i = 1;  // cmd_args[0]은 "echo" 이므로 그 다음부터 출력

    while (cmd_args[i] != NULL) {
        printf("%s", cmd_args[i]);
        if (cmd_args[i + 1] != NULL) {
            printf(" ");
        }
        i++;
    }

    printf("\n");
    return 1;
}
