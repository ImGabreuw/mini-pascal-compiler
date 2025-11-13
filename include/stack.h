#ifndef STACK_H
#define STACK_H

#define MAX 100
#include "binary_tree.h"  

typedef struct {
    char data[MAX];
    int top;
} StackChar;

void pushChar(StackChar *s, char c);
char popChar(StackChar *s);
char peekChar(StackChar *s);
int emptyChar(StackChar *s);

int precedence(char op);


typedef struct {
    Node* data[MAX];
    int top;
} StackNode;

void pushNode(StackNode* s, Node* n);
Node* popNode(StackNode* s);

#endif
