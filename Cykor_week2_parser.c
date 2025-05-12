#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Cykor_week2_parser.h"

//chunk를 이제 명령어 처리를 할 수 있도록 다시 token 단위로 구분
int scan_tokens(const char* chunk, TokenInfo *tokens, int* token_count){

    int i = 0;
    int count = 0;
    int pipe = 0;

    while (chunk[i] != '\0') {
        //공백 무시
        while (isspace(chunk[i])) i++;

        //파이프 감지 시 인덱싱 종료 - 이후 파이프 분기에서 파이프 기준으로 다시 파싱
        if (chunk[i] == '|') {
            if (chunk[i + 1] != '|') {
                count++;
                pipe++;
                return pipe;
            }
        }

        int start = i;

        // 인용부호 처리
        if (chunk[i] == '"' || chunk[i] == '\'') {
            // "로 시작해서 '로 닫히는 경우 방지
            char quote_char = chunk[i++];
            start = i; // 인용부호 다음부터
            while (chunk[i] && chunk[i] != quote_char) i++;
            int end = i - 1; // 닫는 인용부호 전까지

            if (chunk[i] == quote_char) i++;  // 닫는 인용부호 넘기기

            //전부 하나의 파라미터터로 배정
            tokens[count++] = (TokenInfo){PARAM, start, end};
            continue;
        }

        // 일반 파라미터
        start = i;
        while (chunk[i] && !isspace(chunk[i]) && chunk[i] != '|') i++;
        if (i > start) {
            tokens[count++] = (TokenInfo){PARAM, start, i - 1};
        }
    }

    *token_count = count;
    return 0;
}

//입력받은 문자열을 다중 명령어 연산자 기준으로 chunk 단위로 구분
int scan_chunk(const char *input, ChunkInfo *chunk, int *chunk_count) {
    int i = 0;
    int count = 0;

    while (input[i] != '\0') {

        //공백을 건너 뛰는 코드
        if (isspace(input[i])) {
            i++;
            continue;
        }
        
        // |와 ||를 구분
        if (input[i] == '|') {
            if (input[i + 1] == '|') {
                chunk[count++] = (ChunkInfo){OR, i, i + 1};
                i += 2;
            } 
            else {
                count++;
                i++;
            }
        }
        // &와 &&를 구분
        else if (input[i] == '&') {
            if (input[i + 1] == '&') {
                chunk[count++] = (ChunkInfo){AND, i, i + 1};
                i += 2;
            } else {
                chunk[count++] = (ChunkInfo){BACK, i, i};
                i++;
            }
        }
        // ;를 구분
        else if (input[i] == ';') {
            chunk[count++] = (ChunkInfo){SEPERATOR, i, i};
            i++;
            
        }
        //구분자가 아닌 문자열에 대하여 명령어로 인식
        else {
            int start = i;
            while (input[i] && !(input[i] == '|' && input[i+1] == '|') && input[i] != '&' && input[i] != ';') {
                i++;
            }
            chunk[count++] = (ChunkInfo){CHUNK, start, i - 1};
        }
    }

    *chunk_count = count;
    return 0;
}

//파이프 분기 시 입력 받은 청크를 파이프 기호 기준으로 다시 chunk 단위로 구분
int scan_pipe(const char *input, ChunkInfo *chunk, int *chunk_count, int *pipe_count){
    int i = 0;
    int pipe = 0;
    int count = 0;

    while (input[i] != '\0') {

        //공백을 건너 뛰는 코드
        if (isspace(input[i])) {
            i++;
            continue;
        }
        
        // |를 구분
        if (input[i] == '|') {
            if (input[i + 1] != '|') {
                chunk[count++] = (ChunkInfo){PIPE, i, i};
                pipe++;
                i ++;
            } 
        }
      
        //구분자가 아닌 문자열에 대하여 명령어로 인식
        else {
            int start = i;
            while (input[i] &&input[i] != '|') {
                i++;
            }
            chunk[count++] = (ChunkInfo){CHUNK, start, i - 1};
        }
    }

    *chunk_count = count;
    *pipe_count = pipe;
    return 0;
}

//scan_tokens에서 받은 정보를 기반으로 한 chunk를 token으로 나눈 배열 생성
char **build_token_array(const char *input, TokenInfo *tokens, int token_count) {
    char **result = malloc(sizeof(char *) * (token_count + 1));  // 널문자 입력을 위해 1만큼 큰 바이트 할당
    if (!result) return NULL;

    for (int i = 0; i < token_count; i++) {
        int len = tokens[i].end - tokens[i].start + 1; // 널 문자를 집어넣기 위해 길이를 문자열 크기보다 1만큼 크게 설정
        result[i] = malloc(len + 1);
        if (!result[i]) {
            // 해제 후 종료
            for (int j = 0; j < i; j++) free(result[j]);
            free(result);
            return NULL;
        }
        strncpy(result[i], &input[tokens[i].start], len);
        result[i][len] = '\0';  // 널 문자 삽입 후 종료
    }

    result[token_count] = NULL;  // execvp 호환 가능하게
    return result;
}

//scan_chunk에서 받은 정보를 바탕으로 입력을 chunk들로 나눈 배열 생성
char **build_chunk_array(const char *input, ChunkInfo*chunk, int chunk_count) {
    char **result = malloc(sizeof(char *) * (chunk_count + 1));  // 널문자 입력을 위해 1만큼 큰 바이트 할당
    if (!result) return NULL;

    for (int i = 0; i < chunk_count; i++) {
        int len = chunk[i].end - chunk[i].start + 1; // 널 문자를 집어넣기 위해 길이를 문자열 크기보다 1만큼 크게 설정
        result[i] = malloc(len + 1);
        if (!result[i]) {
            // 해제 후 종료
            for (int j = 0; j < i; j++) free(result[j]);
            free(result);
            return NULL;
        }
        strncpy(result[i], &input[chunk[i].start], len);
        result[i][len] = '\0';  // 널 문자 삽입 후 종료
    }

    result[chunk_count] = NULL;  // execvp 호환 가능하게
    return result;
}

// 파싱된 배열에 저장되었던 요소들에 동적 할당 되었던 메모리 해제
void free_token_array(char **array) {
    if (!array) return;
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }
    free(array);
}


