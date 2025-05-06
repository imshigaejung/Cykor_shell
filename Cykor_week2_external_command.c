#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int execute_external_command(char **argv) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork 실패");
        exit(EXIT_FAILURE);
        return 0;
    }

    if (pid == 0) {
        // 자식 프로세스: 외부 명령어 실행
        
        execvp(argv[0], argv);
        // execvp 실패 시에만 아래 코드 실행됨
        perror("명령어 실행 실패");
        exit(EXIT_FAILURE);
    } else {
        // 부모 프로세스: 자식 종료 대기
        int status;
        waitpid(pid, &status, 0);
    }
    return 1;
}
