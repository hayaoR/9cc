#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"


// If the next token is the expected symbol, read one token and return true. 
// Otherwise, returns false.
bool consume(char *op) {
    if (token->kind != TK_RESERVED || 
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

bool consume_kind(TokenKind kind) {
	if (token->kind != kind) {
		return false;
	} else {
		token = token->next;
		return true;
	}

}
// If the next token is the expected symbol, read one token and return true. 
// Otherwise, report error.
void expect(char *op) {
    if (token->kind != TK_RESERVED || 
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error("'%c'ではありません。", op);
    token = token->next;
} 

int expect_number() {
    if (token->kind != TK_NUM)
        error("数ではありません。");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            (c == '_');
}

// generate the new token and connect cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next  = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {

        if (isspace(*p)) {
            p++;
            continue;
        }

        if (!strncmp(p, ">=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, "==", 2) || !strncmp(p, "!=", 2)) {
            cur = new_token(TK_RESERVED, cur, p);
            p+=2;
            cur->len = 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' || *p == '{' || *p == '}') {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        if (!strncmp(p, "return", 6) && !is_alnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p);
            p += 6;
            cur->len = 6;
            continue;
        }

        if (!strncmp(p, "if", 2) && !is_alnum(p[2])) {
            cur = new_token(TK_IF, cur, p);
            p += 2;
            cur->len = 2;
            continue;
        }
		
        if (!strncmp(p, "else", 4) && !is_alnum(p[4])) {
			//fprintf(stderr, "else?\n");
            cur = new_token(TK_ELSE, cur, p);
            p += 4;
            cur->len = 4;
            continue;
        }

        if (!strncmp(p, "while", 5) && !is_alnum(p[5])) {
			//fprintf(stderr, "while?\n");
            cur = new_token(TK_WHILE, cur, p);
            p += 5;
            cur->len = 5;
            continue;
        }

        if (!strncmp(p, "for", 3) && !is_alnum(p[3])) {
            cur = new_token(TK_FOR, cur, p);
            p += 3;
            cur->len = 3;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            int count = 1;
            for (int i = 1; 'a' <= *(p+i) && *(p+i) <= 'z'; i++) {
                count++;
            }
            cur = new_token(TK_IDENT, cur, p);
            cur->len = count;
            p+=count;
            continue;
        }
        //fprintf(stderr, "hi\n");
        error("トークナイズできません。");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}