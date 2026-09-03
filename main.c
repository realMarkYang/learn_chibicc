#include <stdio.h>
#include <stdlib.h>

/*
  .globl main
main:
  mov $5, %rax
  add $20, %rax
  sub $4, %rax
  ret
*/



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