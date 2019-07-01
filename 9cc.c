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

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());

 
    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
        continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf("  ret\n");
    return 0;
} 
