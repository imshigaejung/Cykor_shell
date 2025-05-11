#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

//외장 명령어를 실행하는 함수. execvp와 fork의 조합으로 명령어 처리를 수행한다.
int execute_external_command(char **argv, int is_background, int *job_id) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failure");
        exit(EXIT_FAILURE);
        return 0;
    }

    if (pid == 0) {
        // 자식 프로세스: 외부 명령어 실행
        fflush(stdout);
        fflush(stderr);
        execvp(argv[0], argv);
        // execvp 실패 시에만 아래 코드 실행됨
        perror("execute failure");
        exit(EXIT_FAILURE);
    } else {
        if(!is_background){
        // 부모 프로세스: 자식 종료 대기
        int status;
        waitpid(pid, &status, 0);
        }
        else{
            printf("[%d] %d\n", *job_id++, pid);
            fflush(stdout);
        }

    }
    return 1;
}
