#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

// Used to give a unique label
int SERIAL = 0;

//左辺値として式を評価する
//スタックに変数のアドレスが積まれる
void get_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません。");
    
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n"); 
}

// emulate the stack machine in x86-64
/*
	rsp: used as the stack pointer
	On x86-64, the result of the compare instruction is set in a special "flag register".

	sete instruction: If the values of the two registers checked by the previous cmp instruction are the same, 
						set 1 to the specified register (here, AL). Otherwise, set to 0.

	al: alias register that is lower 8 bits of RAX

	movzb instruction: move with zero extend.

	rsp: designed to be used as a stack pointer

	rbp: base register Always points to the start of the current function frame.

	mov rdi, [rax] -> load value from address in RAX and set to RDI.

	mov [rdi], rax -> store the value of RAX at the address in RDI.

	rax: The value stored in RAX when returning from the function is the return value of the function

*/
/*
 抽象木をコンパイルするためには
 1. 左の部分木をコンパイルする
 2.　右の部分木をコンパイルする
 3.　スタックの2つの値を、それらを加算した結果で置き換えるコードを出力する
*/
/*
 x86-64のpushやpopといった命令は暗黙のうちにRSPをスタックポインタとして使って、
 その値を変更しつつ、RSPが指しているメモリにアクセスする命令です。
*/
void gen(Node *node) {
	//シリアル番号を保持するために使う
	int tmp;
	//関数の引数のためのレジスタ
	char reg[6][4] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
	char *str;
    switch (node->kind)
    {
	case ND_FUNCTIONDECLARATION:
		str = calloc(node->len+1, sizeof(char));
		memcpy(str, node->name, node->len);
		str[node->len] = '\0';
		printf("%s:\n", str);
		// Prologue
    	printf("    push rbp\n");
    	printf("    mov rbp, rsp\n");
    	printf("    sub rsp, 208\n");

		//引数
		printf("    mov rax, rbp\n");	
		for (int i = 0; i < node->arg_len; ++i) {
			printf("    mov rax, rbp\n");	
			printf("    sub rax, %d\n", node->arg_offset[i]);	
    		printf("    mov [rax], %s\n", reg[i]);
		}


		//ボディ
		for (int i = 0; i < node->block_len; ++i) {
			gen(node->block[i]);
        	printf("    pop rax\n");
		}

		//Epilogue
    	printf("    mov rsp, rbp\n");
    	printf("    pop rbp\n");
    	printf("    ret\n");
		return;
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        get_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        get_lval(node->lhs);
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n"); 
        return;
	case ND_RETURN:
		gen(node->lhs);
		/*
		if (node->lhs->kind != ND_FUNCTIONCALL) {
   	    	printf("    pop rax\n");
		}
		*/
   	    printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    	return;
	case ND_IF:
		tmp = ++SERIAL;
		gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", tmp);
		gen(node->rhs->lhs);
        printf(".Lend%d:\n", tmp);
		return;
	case ND_IFELSE:
		tmp = ++SERIAL;
		gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lelse%d\n", tmp);
		gen(node->rhs->lhs);
        printf("    jmp .Lend%d\n", tmp);
        printf(".Lelse%d:\n", tmp);
		gen(node->rhs->rhs);
        printf(".Lend%d:\n", tmp);

		return;
	case ND_WHILE:
		tmp = ++SERIAL;
		printf(".Lbegin%d:\n", tmp);
		gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", tmp);
		gen(node->rhs);
        printf("    jmp .Lbegin%d\n", tmp);
        printf(".Lend%d:\n", tmp);
		return;
	case ND_FOR:
		//fprintf(stderr, "FOR!\n");
		tmp = ++SERIAL;
		gen(node->lhs); // A
		printf(".Lbegin%d:\n", tmp);
		gen(node->rhs->lhs); // B
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", tmp);
		gen(node->rhs->rhs->rhs); // D
		gen(node->rhs->rhs->lhs); // C
        printf("    jmp .Lbegin%d\n", tmp);
        printf(".Lend%d:\n", tmp);
		return;
	case ND_BLOCK:
		for (int i = 0; i < node->block_len; ++i) {
			gen(node->block[i]);
        	printf("    pop rax\n");
		}
		return;
	case ND_FUNCTIONCALL:
		for (int i = 0; i < node->arg_len; ++i) {
			gen(node->arg[i]);
        	printf("    pop rax\n");
        	printf("    mov %s, rax\n", reg[i]);
		}
		char* str = calloc(node->len+1, sizeof(char));
		memcpy(str, node->name, node->len);
		str[node->len] = '\0';
		
		//16bit alignmentの実装はruiさんのchibiccに沿った
		int tmp = ++SERIAL;
        printf("    mov rax, rsp\n");
        printf("    and rax, 15\n");
        printf("    jnz .L.call.%d\n", tmp);
        printf("    mov rax, 0\n");
        printf("    call %s\n", str);
        printf("    jmp .L.end.%d\n", tmp);
        printf(".L.call.%d:\n", tmp);
        printf("    sub rsp, 8\n");
        printf("    mov rax, 0\n");
        printf("    call %s\n", str);
        printf("    add rsp, 8\n");
        printf(".L.end.%d:\n", tmp);

		//スタックトップにプッシュするpopしてもらわないと困る
        printf("    push rax\n");
		return;
    }

/*
    gen(node->lhs);
   	gen(node->rhs);

	if (node->lhs->kind != ND_FUNCTIONCALL) {
    	printf("    pop rdi\n");
	} else {
        printf("    mov rdi, rax\n");
		fprintf(stderr, "lhs\n");
	}
	if (node->rhs->kind != ND_FUNCTIONCALL) {
    	printf("    pop rax\n");
	} else {
		fprintf(stderr, "rhs\n");
	}
*/
	
    gen(node->lhs);
    gen(node->rhs);

   	printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax ,al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
    }

    printf("    push rax\n");
}