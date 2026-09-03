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

// 确保跳过的就是s
static Token *skip(Token *tok, char *s)
{
    if (!equal(tok, s))
        error("except '%s'", s);
    return tok->next;
}

static int get_number(Token *tok)
{
    if (tok->kind != TK_NUM)
        error("excepted a number");
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
static Token *tokenize(char *p)
{
    Token head = {};
    Token *cur = &head;

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

        if (*p == '+' || *p == '-')
        {
            cur = cur->next = new_token(TK_PUNCT, p, p + 1);
            p++;
            continue;
        }

        error("invalid token");
    }

    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        error("%s: invalid number of arguments\n", argv[0]);

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