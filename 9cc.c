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
	
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

	function();
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

    }

    return 0;
} 
