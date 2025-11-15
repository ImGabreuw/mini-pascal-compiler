#include <ctype.h>

#include "stack.h"
#include "postfix_converter.h"

void infixToPostfix(char infix[], char postfix[]) {
    StackChar stack;
    stack.top = -1;

    int j = 0;
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];

        if (isspace(c))
            continue;

        if (isalnum(c)) {                  
            postfix[j++] = c;
        }
        else if (c == '(') {
            pushChar(&stack, c);
        }
        else if (c == ')') {
            while (!emptyChar(&stack) && peekChar(&stack) != '(')
                postfix[j++] = popChar(&stack);
            popChar(&stack);               
        }
        else {                             
            while (!emptyChar(&stack) &&
                   precedence(peekChar(&stack)) >= precedence(c))
                postfix[j++] = popChar(&stack);
            pushChar(&stack, c);
        }
    }

    while (!emptyChar(&stack))
        postfix[j++] = popChar(&stack);

    postfix[j] = '\0';
}
