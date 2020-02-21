#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません。");
        return 1;
    }

    user_input = argv[1]; //エラー関数用

	// tokenize and parse
	// the result is stored in code
    token = tokenize(argv[1]);
    program();
    
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Prologue: fixed instruction output by the compiler at the beginning of the function.
	// why 208? -> to allocate space for all single-character variables. 26*8 = 208
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        
        printf("    pop rax\n");
    }

    // Epilogue: fixed instructions output at the end of the function.
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
} 
