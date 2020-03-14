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

// ローカル変数を連結リスト 
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
//  変数を名前で検索する見つからなかった場合はNULLを返す。
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
		// if (A) B else C
		node = calloc(1, sizeof(Node));
		expect("(");
		node->lhs = expr();
		expect(")");
		Node *child;
		child= calloc(1, sizeof(Node));
		child->lhs = stmt();
		if (consume_kind(TK_ELSE)) {
			node->kind = ND_IFELSE;
			child->rhs = stmt();
			//child->rhs = stmt();
		} else {
			node->kind = ND_IF;
		}
		node->rhs = child;
	} else if (consume_kind(TK_WHILE)) {
		/*
			while (A) B
		*/
		node = calloc(1, sizeof(Node));
		node->kind=ND_WHILE;

		expect("(");
		node->lhs = expr(); // A
		expect(")");
		node->rhs = stmt(); // B
	} else if (consume_kind(TK_FOR)) {
		/* 
			for (A; B; C)
					D;
		*/
		Node *child, *grandchild;

		node = calloc(1, sizeof(Node));
		child= calloc(1, sizeof(Node));
		grandchild =calloc (1, sizeof(Node));
		//set kind
		node->kind = ND_FOR;

		// construct tree
		node->rhs = child;
		child->rhs = grandchild;

		expect("(");
		node->lhs = stmt(); //A	
		child->lhs = stmt();//B
		grandchild->lhs = expr();//C
		expect(")");
		grandchild->rhs = stmt();//D

	} else if (consume_kind(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = stmt();
    } else if (consume("{")) {
        node = calloc(1, sizeof(Node));
		for (int i = 0; !consume("}") && i < 100; ++i) {
			Node* tmp_node;
			tmp_node = stmt();
			node->block[i] = tmp_node;
			node->block_len = i+1;
			//fprintf(stderr, "get\n");
		}
		node->kind = ND_BLOCK;
	} else {
        node = expr();   
    	if (!consume(";"))
    	    error_at(token[pos].str,"';' ではないトークンです");
    }

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
	Node *node;
    if (consume("(")) {
        node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
		if (consume("(")) {
			//fprintf(stderr, "hey\n");
			//関数呼び出し
			node = calloc(1, sizeof(Node));
			node->kind = ND_FUNCTION;
			node->name = tok->str;
			node->len = tok->len;
			node->arg_len = 0;

			for (int i = 0; !consume(")"); i++) {
				if (i > 5) {
 					error_at(token[pos].str,"引数は6個以下です。");
				}
				Node* tmp = calloc(1, sizeof(Node));
				tmp = expr();
				node->arg[i] = tmp;
				node->arg_len = i+1;

				consume(",");
			}

			//fprintf(stderr, "aftertok\n");
			return node;
		} else {
			//多分変数解決?
			node = calloc(1, sizeof(Node));
			node->kind = ND_LVAR;

			if (locals == NULL) {
				LVar *l = calloc(1, sizeof(LVar));
				locals = l;
			}
			LVar *lvar = find_lvar(tok);
			
			if (lvar) {
			//登録済みのローカル変数
				node->offset = lvar->offset;
			} else {
			//未登録のローカル変数
				lvar = calloc(1, sizeof(LVar));
			//ローカル変数の連結リストに接続
				lvar->next = locals;
				lvar->name = tok->str;
				lvar->len = tok->len;
				lvar->offset = locals->offset + 8;
				node->offset = lvar->offset;
				locals = lvar;
			}
		}
		return node;
    }

    return new_node_num(expect_number());
}

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
	char reg[6][4] = {"RDI", "RSI", "RDX", "RCX", "R8", "R9"};
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
	case ND_RETURN:
		gen(node->lhs);
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
	case ND_FUNCTION:
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