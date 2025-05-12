#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "Cykor_week2_parser.h"
#include "Cykor_week2_executor.h"
#include <dirent.h>
#include <termios.h>
#include <pwd.h>

#define MAX_CHUNK 120

//사용자 정보 관리용 구조체
typedef struct user_info
{
    char user_name[20];
    char device_name[20];
}info;

//종료된 모든 자식 프로세스를 비동기로 수거함 - 좀비 프로세스 방지 (백그라운드 실행을 부모가 wait 하지 않는 형태로 구현 했으므로 반드시 필요!)
void sigchld_handler(int sig) {
    // 종료된 모든 자식 프로세스(-1)를 비동기(WNOHANG)로 수거
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

//환경변수를 이용해 사용자의 기기정보와 이름을 불러옴
void Login(info *pointer){
    
    //사용자 이름 호출
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        strncpy(pointer->user_name, pw->pw_name, sizeof(pointer->user_name));
    }

    //기기 정보 호출
    gethostname(pointer->device_name, sizeof(pointer->device_name));

}

//사용자로부터 직접 입력을 받는 함수. 오버플로우를 막기 위해 동적할당으로 입력을 받는다
char* dynamic_input(){

    //최초 크기 128 할당
    size_t buffer_size = 128;
    char * buffer = malloc(buffer_size);
    if(!buffer)
        return NULL;
    
    int input;
    size_t len = 0;

    //getchar의 반환값이 int이므로 input은 int 형으로 선언 (EOF를 감지해야 하기 때문!)
    while ((input = getchar()) != '\n' && input != EOF){
        if (len+1>= buffer_size){
            buffer_size *= 2;
            char*new_buffer = realloc(buffer, buffer_size);
            
            //할당 실패 시 메모리 해제
            if(!new_buffer){
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[len++] = (char)input;
        }
        //개행문자 제거
        buffer[len] = '\0';
        return buffer;
    }

//프롬프트 출력의 경우 exit이 입력되기 전까지 반복 호출되므로 함수로 정리해서 가독성 향상
void print_prompt(info user_informaition) {

    char *cwd = getcwd(NULL, 0);
    char *home = getenv("HOME");
  
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        // 홈 디렉토리일 경우 '~'로 축약
        printf("%s@%s:~%s$ ", user_informaition.user_name, user_informaition.device_name, cwd + strlen(home));
    } else {
        printf("%s@%s:%s$ ", user_informaition.user_name, user_informaition.device_name, cwd);
    }
}

//메인
int main(){
    char *input;
    char **chunk_array;
    //사용자 정보는 혹시나 잘못 초기화 되는 일 없도록 static 선언
    static info user_information; 
    int chunk_count = 0;

    //해당 배열 또한 동적으로 그 크기를 조절하려 했으나, 메모리가 오염되는 버그를 해결하지 못해 원래대로 되돌림림
    ChunkInfo chunk[MAX_CHUNK];

    //사용자 정보 저장
    Login(&user_information);

    //자식 프로세스 종료 이후 SIGCHLD 신호가 전달되면 sighcld_handler 호출
    signal(SIGCHLD, sigchld_handler);
    
    while(1){
    //명령어 입력 & 개행문자 제거
        print_prompt(user_information);
        input = dynamic_input();

        //아무것도 입력이 되지 않았을 시 세그폴트가 일어날 가능성 대비하는 코드
        if (!input || input[0] == '\0') {
            free(input);
            
            continue; 
        }
        
        //문자열 스캔 --> 구분자, 공백, 명령어 구분
        scan_chunk(input, chunk, &chunk_count);

        //스캔한 문자열을 토큰화 시켜서 parsed_array에 저장
        chunk_array = build_chunk_array(input, chunk, chunk_count);

        //이제 input에 할당 되었던 메모리 다시 해제
        free(input);
        
        //토큰들을 명령어 처리 함수로 넘김
        command_branch(chunk_array, chunk, chunk_count,0);
        //청크들 동적할당 해뒀던 메모리 해제
        free_token_array(chunk_array);

        //백그라운드 실행 시 출력 순서 경쟁을 해결하기 위해 프롬프트 출력을 잠시 늦춤
        fflush(stdout);
        usleep(10000);
    }
}
