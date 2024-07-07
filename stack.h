#ifndef STACK_H
#define STACK_H

#define MAX 100

typedef struct Stack {
    char* items[MAX];
    int top;
} Stack;

void initialize(Stack* s);
int isEmpty(Stack* s);
int isFull(Stack* s);
void push(Stack* s, const char* value);
char* pop(Stack* s);
char* peek(Stack* s);
void display(Stack* s);

#endif // STACK_H
