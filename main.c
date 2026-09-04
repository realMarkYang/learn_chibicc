#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
  .globl main
main:
  mov $5, %rax
  add $20, %rax
  sub $4, %rax
  ret
*/
typedef enum
{
    TK_PUNCT, // 标点符号
    TK_NUM,   // 数字
    TK_EOF    // 结束
} TokenKind;

typedef struct Token Token;
struct Token
{
    TokenKind kind;
    Token *next;
    int val;
    char *loc;
    int len;
};


static char *current_input;

static bool equal(Token *tok, char *op)
{
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// 使用可变参数列表来
static void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1); // 错误退出
}


//能指出错误在哪里的error
static void verror_at(char *loc, char *fmt, va_list ap)
{
    int pos = loc - current_input;
    fprintf(stderr, "%s\n", current_input);
    fprintf(stderr, "%*s", pos, ""); // print pos spaces.
    fprintf(stderr,"^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

static void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}



// 确保跳过的就是s
static Token *skip(Token *tok, char *s)
{
    if (!equal(tok, s))
        error_tok(tok,"except '%s'", s);
    return tok->next;
}

static int get_number(Token *tok)
{
    if (tok->kind != TK_NUM)
        error_tok(tok,"excepted a number");
    return tok->val;
}

static Token *new_token(TokenKind kind, char *start, char *end)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

// file start *p
static Token *tokenize()
{
    Token head = {};
    Token *cur = &head;
    char *p = current_input; 

    while (*p)
    {
        // skip whitespace characters
        if (isspace(*p))
        {
            p++;
            continue;
        }

        // parse number
        if (isdigit(*p))
        {
            cur = cur->next = new_token(TK_NUM, p, p);
            char *q = p;
            cur->val = strtoul(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if (ispunct(*p))
        {
            cur = cur->next = new_token(TK_PUNCT, p, p + 1);
            p++;
            continue;
        }

        error_at(p,"invalid token\n");
    }

    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}

// Parser
// Preper to build a AST_Node Tree
typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // Integer
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node
{
    NodeKind kind; // Node kind
    Node *lhs;     // Left-hand side
    Node *rhs;     // Right-hand side
    int val;       // Used if kind == ND_NUM
};


static Node *new_node(NodeKind kind)
{
    Node *node = calloc(1,sizeof(Node));
    node->kind = kind;
    return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node  *rhs)
{
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_num(int val)
{
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

static Node *expr(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

// expr = mul ("+" mul | "-" mul)*
static Node *expr(Token **rest, Token *tok)
{
    //mul first
    Node *node = mul(&tok,tok);

    for(;;)
    {
        if(equal(tok,"+"))
        {
            node = new_binary(ND_ADD,node,primary(&tok,tok->next));
            continue;
        }

        if (equal(tok, "-"))
        {
            node = new_binary(ND_SUB, node, primary(&tok, tok->next));
            continue;
        }
    
        *rest = tok;
        return node;
    }
}

// mul = primary ("*" primary | "/" primary)*
static Node *mul(Token **rest,Token *tok)
{
    Node *node = primary(&tok,tok);
    for(;;)
    {
        if(equal(tok,"*"))
        {
            node = new_binary(ND_MUL,node,primary(&tok,tok->next));
            continue;
        }

        if(equal(tok,"/"))
        {
            node = new_binary(ND_DIV,node,primary(&tok,tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }

}

// primary = "(" expr ")" | num
static Node *primary(Token **rest,Token *tok)
{
    if(equal(tok,"("))
    {
        Node *node = expr(&tok,tok->next);
        *rest = skip(tok,")");
        return node;
    }

    if(tok->kind == TK_NUM)
    {
        Node *node = new_num(tok->val);
        *rest = tok->next;
        return node;
    }

    error_tok(tok, "expected an expression");

}




int main(int argc, char **argv)
{
    if (argc != 2)
        error("%s: invalid number of arguments\n", argv[0]);

    
    current_input = argv[1];
    // parse token
    Token *tok = tokenize(argv[1]);

    printf("  .globl main\n");
    printf("main:\n");
    printf("  mov $%d, %%rax\n", get_number(tok));
    tok = tok->next;

    while (tok->kind != TK_EOF)
    {
        if (equal(tok, "+"))
        {
            printf("  add $%d, %%rax\n", get_number(tok->next));
            tok = tok->next->next;
            continue;
        }

        tok = skip(tok, "-");
        printf("  sub $%d, %%rax\n", get_number(tok));
        tok = tok->next;
    }

    printf("  ret\n");
    return 0;
}