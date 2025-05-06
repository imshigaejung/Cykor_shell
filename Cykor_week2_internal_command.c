#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


int execute_cd(char **cmd_args) {
    if (cmd_args[1] == NULL) {
        // 인자 없으면 HOME 디렉토리로 이동
        char *home = getenv("HOME");
        if (home != NULL) {
            if (chdir(home) != 0) {
                perror("cd");
                return 0;
            }
            return 1;
        } else {
            fprintf(stderr, "cd: HOME not set\n");
            return 0;
        }
    } else {
        // 인자(args[1])는 동적 할당된 문자열
        if (chdir(cmd_args[1]) != 0) {
            perror("cd");
            return 0;
        }
        return 1;
    }
}

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

int execute_exit(){
    exit(0);
}
