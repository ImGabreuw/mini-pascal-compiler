#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "binary_tree.h"
#include "stack.h"

Node* newNode(char c) {
    Node* n = malloc(sizeof(Node));
    n->value = c;
    n->left = n->right = NULL;
    return n;
}

Node* buildExpressionTree(char postfix[]) {
    StackNode stack;
    stack.top = -1;

    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];

        if (isalnum(c)) {
            pushNode(&stack, newNode(c));
        } else {
            Node* right = popNode(&stack);
            Node* left  = popNode(&stack);
            Node* op = newNode(c);
            op->left = left;
            op->right = right;
            pushNode(&stack, op);
        }
    }

    return popNode(&stack);
}

void printPreorder(Node* root) {
    if (!root) return;
    printf("%c ", root->value);
    printPreorder(root->left);
    printPreorder(root->right);
}

void printInorder(Node* root) {
    if (!root) return;
    printInorder(root->left);
    printf("%c ", root->value);
    printInorder(root->right);
}

void printPostorder(Node* root) {
    if (!root) return;
    printPostorder(root->left);
    printPostorder(root->right);
    printf("%c ", root->value);
}

void drawTree(Node* root, int level) {
    if (!root) return;
    drawTree(root->right, level + 1);
    for (int i = 0; i < level; i++) printf("   ");
    printf("%c\n", root->value);
    drawTree(root->left, level + 1);
}
