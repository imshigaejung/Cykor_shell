#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Cykor_week2_parser.h"

//입력받은 문자열을 문자 단위로 분석 --> 
int scan_tokens(const char *input, TokenInfo *tokens, int *token_count) {
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
                tokens[count++] = (TokenInfo){OR, i, i + 1};
                i += 2;
            } else {
                tokens[count++] = (TokenInfo){PIPE, i, i};
                i++;
            }
        }
        // &와 &&를 구분
        else if (input[i] == '&') {
            if (input[i + 1] == '&') {
                tokens[count++] = (TokenInfo){AND, i, i + 1};
                i += 2;
            } else {
                tokens[count++] = (TokenInfo){BACK, i, i};
                i++;
            }
        }
        //구분자가 아닌 문자열에 대하여 명령어로 인식
        else {
            int start = i;
            while (input[i] && !isspace(input[i]) && input[i] != '|' && input[i] != '&') {
                i++;
            }
            tokens[count++] = (TokenInfo){CMD_NONE, start, i - 1};
        }
    }

    *token_count = count;
    return 0;
}

char **build_token_array(const char *input, TokenInfo *tokens, int token_count) {
    char **result = malloc(sizeof(char *) * (token_count + 1));  // 널문자 입력을 위해 1만큼 큰 바이트 할당
    if (!result) return NULL;

    for (int i = 0; i < token_count; i++) {
        int len = tokens[i].end - tokens[i].start + 1; // 널 문자를 집어넣기 위해 길이를 문자열 크기보다 1만큼 크게 설정정
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


// token_array에 저장되었던 토큰들에게 동적 할당 되었던 메모리 해제
void free_token_array(char **array) {
    if (!array) return;
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }
    free(array);
}