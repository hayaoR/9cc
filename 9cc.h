#ifndef CC_H
#define CC_H


typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};
Token *token;


typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_LVAR,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

char *user_input;

Node *code[100];

/* error.c */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

/* tokenizer.c */
bool consume(char *op);
Token* consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

Token *tokenize(char *p);

/* generate.c */
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
void gen(Node *node);

#endif