#ifndef CC_H
#define CC_H

///////////////// used in code -> tokens //////////////////////////////////
// Token type
typedef enum {
    TK_RESERVED, // symbol
    TK_IDENT, // multi-character identifier
    TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind; 
    Token *next; // next input token
    int val; // if the toke is TK_NUM, the number. 
    char *str; // Token str 
    int len;
};
// currently focused token
Token *token;
///////////////// used in code -> tokens //////////////////////////////////

///////////////// used in tokens -> ast  //////////////////////////////////
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_LVAR, //ローカル変数
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
    ND_RETURN,
	ND_IF,
	ND_IFELSE,
	ND_WHILE,
	ND_FOR,
	ND_BLOCK,
	ND_FUNCTIONCALL,
	ND_FUNCTIONDECLARATION,
} NodeKind;

typedef struct Node Node;

/*
	offset: member that represents the offset of the local variable from the base pointer.
*/
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
	//ハードコードするのは良くない。できればC++でいうvectorのようなものが望ましい
	//1000個以上stmtがあるとバグる
	Node *block[1000]; // used only when kind is ND_BLOCK
	int block_len; // used only when kind is ND_BLOCK 
    int val; // used only when kind is ND_NUM
    int offset; // used only when kind is ND_LVAR

	//関数用
	char* name;
	int len;
	Node *arg[6]; // used as argument of a function only when kind is ND_FUNCTION
	int arg_len; // used as argument of a function only when kind is ND_FUNCTION
	int arg_offset[6];
};

int pos;

char *user_input;

Node *code[100];
///////////////// used in tokens -> ast  //////////////////////////////////

/* error.c */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

/* tokenizer.c */
bool consume(char *op);
Token* consume_ident();
bool consume_kind(TokenKind kind);
void expect(char *op);
int expect_number();
bool at_eof();

Token *tokenize(char *p);

/* generate.c */
void function();
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