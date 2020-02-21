#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

// Used to give a unique label
int SERIAL = 0;

typedef struct LVar LVar;

// local variable.
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

// keep variables.
LVar *locals = NULL;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

/*
	int memcmp( const void *str1 , const void *str2, size_t len );
	if str1 == str2 (len characters) -> 0
	else if str1 > str2 -> positive integer
	else if str1 < str2 -> postive integer
*/
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    }
    return NULL;
}

Token* consume_ident() {
    if (token->kind !=  TK_IDENT)
        return NULL;
    Token* tmp = token;
    token = token->next;
    return tmp;
}

void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
    //fprintf(stderr, "i = %d\n", i);
}

Node *stmt() {
    Node *node;
	if (consume_kind(TK_IF)) {
		node = calloc(1, sizeof(Node));
		expect("(");
		node->lhs = expr();
		expect(")");
		Node *tmp;
		tmp = calloc(1, sizeof(Node));
		tmp->lhs = stmt();
		if (consume_kind(TK_ELSE)) {
			node->kind = ND_IFELSE;
			tmp->rhs = stmt();
		} else {
			//fprintf(stderr,"here?\n");
			node->kind = ND_IF;
		}
		node->rhs = tmp;
	} else if (consume_kind(TK_WHILE)) {

	} else if (consume_kind(TK_FOR)) {

	} else if (consume_kind(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();   
    }

    if (!consume(";"))
        error_at(token[pos].str,"';' ではないトークンです");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) 
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
    if (consume("+"))
        return term();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), term());
    return term();
}

Node *term() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        if (locals == NULL) {
            LVar *l = calloc(1, sizeof(LVar));
            locals = l;
        }
        LVar *lvar = find_lvar(tok);
        
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));

            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}

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

	al: Alias register that is lower 8 bits of RAX

	movzb instruction: move with zero extend.

	rbp: base register Always points to the start of the current function frame.

	mov rdi, [rax] -> load value from address in RAX and set to RDI.

	mov [rdi], rax -> store the value of RAX at the address in RDI.
*/
void gen(Node *node) {
    if (node->kind == ND_RETURN) {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    	return;
    }
	if (node->kind == ND_IF) {
		int tmp = ++SERIAL;
		gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", tmp);
		gen(node->rhs->lhs);
        printf(".Lend%d:\n", tmp);
		return;
	}
	if (node->kind == ND_IFELSE) {
		int tmp = ++SERIAL;
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
	}
	
    switch (node->kind)
    {
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
        printf("    push rdi\n"); //このpushの意味は？
        return;
    }

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