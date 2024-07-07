#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

void initialize(Stack* s) {
    s->top = -1;
}

int isEmpty(Stack* s) {
    return s->top == -1;
}

int isFull(Stack* s) {
    return s->top == MAX - 1;
}

void push(Stack* s, const char* value) {
    if (isFull(s)) {
        printf("Stack is full!\n");
    } else {
        s->items[++(s->top)] = strdup(value);
    }
}

char* pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack is empty!\n");
        return NULL;
    } else {
        char* value = s->items[s->top];
        s->items[s->top--] = NULL;
        return value;
    }
}

char* peek(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack is empty!\n");
        return NULL;
    } else {
        return s->items[s->top];
    }
}

void display(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack is empty!\n");
    } else {
        printf("Stack elements: ");
        for (int i = 0; i <= s->top; i++) {
            printf("%s ", s->items[i]);
        }
        printf("\n");
    }
}
