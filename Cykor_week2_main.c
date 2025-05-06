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

// 현재 토큰의 개수는 정적으로 선언되어있지만, 후에 동적으로 바꿔야 함.

#define MAX_CHUNK 120
const char* token_type_to_str(ChunkType type);

typedef struct user_info
{
    char user_name[20];
    char device_name[20];
}info;

void Login(info *pointer){
    
    //사용자 이름 호출
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        strncpy(pointer->user_name, pw->pw_name, sizeof(pointer->user_name));
    }

    //기기 정보 호출
    gethostname(pointer->device_name, sizeof(pointer->device_name));

}

char* dynamic_input(){

    size_t buffer_size = 128;
    char * buffer = malloc(buffer_size);
    if(!buffer)
        return NULL;
    
    int input;
    size_t len = 0;

    while ((input = getchar()) != '\n' && input != EOF){
        if (len+1>= buffer_size){
            buffer_size *= 2;
            char*new_buffer = realloc(buffer, buffer_size);
            
            if(!new_buffer){
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[len++] = (char)input;
        }
        buffer[len] = '\0';
        return buffer;
    }

void print_prompt(info user_informaition) {

    char *cwd = getcwd(NULL, 0);
    char *home = getenv("HOME");
  
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        // 홈 디렉토리 하위일 경우 '~'로 축약
        printf("%s@%s:~%s$ ", user_informaition.user_name, user_informaition.device_name, cwd + strlen(home));
    } else {
        printf("%s@%s:%s$ ", user_informaition.user_name, user_informaition.device_name, cwd);
    }
}

int main(){
    char *input;
    char **chunk_array;
    static info user_information; 
    ChunkInfo chunk[MAX_CHUNK];
    

    
    int chunk_count = 0;


    Login(&user_information);
    while(1){
    //명령어 입력 & 개행문자 제거
        print_prompt(user_information);
        input = dynamic_input();
    
        //문자열 스캔 --> 구분자, 공백, 명령어 구분
        scan_chunk(input, chunk, &chunk_count);

        //스캔한 문자열을 토큰화 시켜서 parsed_array에 저장
        chunk_array = build_chunk_array(input, chunk, chunk_count);

        //이제 input에 할당 되었던 메모리 다시 해제
        free(input);
        
        /*//명령어를 내장 명령어, 외부 명령어, 지시 사항으로 분리
        scan_command(chunk_array, chunk, chunk_count);*/

        //토큰들을 명령어 처리 함수로 넘김
        execute_command(chunk_array, chunk, chunk_count,0);

        //입력된 문자열이 잘 파싱되었는지 확인 차 출력
        printf("Parsed Array:\n");
        for (int i = 0; chunk_array[i] != NULL; i++) {
        printf("  [%d]: %s - %s\n", i, chunk_array[i], token_type_to_str(chunk[i].type));

        }
    }

}
const char* token_type_to_str(ChunkType type) {
    switch(type) {
        case SEPERATOR: return "SEPERATOR";
        case CHUNK: return "CHUNK";
        case AND: return "AND";
        case OR: return "OR";
        case BACK: return "BACK";
        case PIPE: return "PIPE";
        default: return "UNKNOWN";
    }
}
