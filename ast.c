#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

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

void function() {	
	Token *tok;
	//local初期化
	LVar *l = calloc(1, sizeof(LVar));
	locals = l;

	int i = 0;
	while (!at_eof()) {
    	tok = consume_ident();
		if (!tok) {
 			error_at(token[pos].str,"関数ではありません");
		}
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_FUNCTIONDECLARATION;
		node->name = tok->str;
		node->len = tok->len;

		code[i++] = node;
		//引数
		expect("(");
		int arg_len = 0;
		while(!consume(")")) {
    		tok = consume_ident();
			if (!tok) {
 				error_at(token[pos].str,"引数にふさわしくありません");
			}
			consume(",");

			LVar *lvar = calloc(1, sizeof(LVar));
			//ローカル変数の連結リストに接続
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			lvar->offset = locals->offset + 8;
			node->arg_offset[arg_len] = lvar->offset;
			locals = lvar;

			
			arg_len+=1;
		}
		//引数の個数
		node->arg_len = arg_len;
		expect("{");

		//ボディ
		int j = 0;
		while (!consume("}")) {
			node->block[j++] = stmt();
		}
		node->block_len = j;
		

	}
	code[i] = NULL;
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
			//関数呼び出し
			node = calloc(1, sizeof(Node));
			node->kind = ND_FUNCTIONCALL;
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

			return node;
		} else {
			//変数解決
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