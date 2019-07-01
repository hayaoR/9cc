#ifndef CC_H
#define CC_H

typedef struct Token Token;
struct Token;
Token *token;

void error(char *fmt, ...);
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();
Token *tokenize(char *p);

#endif