#ifndef CC_H
#define CC_H


typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};
Token *token;


typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;


typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

char *user_input;

/* error.c */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

/* func.c */
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();

Token *tokenize(char *p);

/* generate.c */
Node *expr();
Node *mul();
Node *term();
void gen(Node *node);

#endif