#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

/*
  .globl main
main:
  mov $5, %rax
  add $20, %rax
  sub $4, %rax
  ret
*/
typedef enum{
    TK_PUNCT,   //标点符号
    TK_NUM,     //数字
    TK_EOF      //结束
} TokenKind;

typedef struct Token
{
    TokenKind kind;
    Token *next;
    int val;
    char *loc;
    int len;
} Token;

static bool equal(Token *tok, char *op)
{
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

static void error(char *fmt,...)
{
    va_list ap;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        return 1;
    }

    
    char *p = argv[1];
    printf("  .global main\n");
    printf("main:\n");
    printf("  mov $%ld, %%rax\n",strtol(p,&p,10));

    while(*p)
    {
        if(*p=='+')
        {
            p++;
            printf("  add $%ld, %%rax\n",strtol(p,&p,10));
            continue;
        }
        
        if(*p=='-')
        {
            p++;
            printf("  sub $%ld, %%rax\n",strtol(p,&p,10));
            continue;
        }

        fprintf(stderr,"unknown character: %c\n",*p);
        return 1;
    }

    printf("  ret\n");
    return 0;
}