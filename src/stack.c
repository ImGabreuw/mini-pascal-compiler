#include <stdio.h>
#include <ctype.h>

#include "stack.h"

void pushChar(StackChar *s, char c)
{
    s->data[++s->top] = c;
}

char popChar(StackChar *s)
{
    return s->data[s->top--];
}

char peekChar(StackChar *s)
{
    return s->data[s->top];
}

int emptyChar(StackChar *s)
{
    return s->top == -1;
}

int precedence(char op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

void pushNode(StackNode *s, Node *n)
{
    s->data[++s->top] = n;
}

Node *popNode(StackNode *s)
{
    return s->data[s->top--];
}
