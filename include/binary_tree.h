#ifndef BIN_TREE_H
#define BIN_TREE_H

// Estrutura do nó da árvore
typedef struct Node {
    char value;
    struct Node *left, *right;
} Node;

// Criação de nó
Node* newNode(char c);

// Construção da árvore a partir da pós-fixa
Node* buildExpressionTree(char postfix[]);

// Impressões
void printPreorder(Node* root);
void printInorder(Node* root);
void printPostorder(Node* root);

// Desenho da árvore
void drawTree(Node* root, int level);

#endif
