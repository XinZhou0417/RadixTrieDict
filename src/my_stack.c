/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
07 Sep 2023
..Project: Assignment2
my_stack.c :
        = Implementation of Stack
----------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "my_stack.h"

typedef struct StackNode StNode;
struct StackNode {
    void* val;
    StNode* next;
};


struct MyStack {
    StNode* head;
    size_t size;
};


// Get the size of a stack  
size_t getStackSize(Stack* stack) {
    return stack->size;
}


// get a new stack
Stack* newStack() {
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    assert(stack);
    stack->head = NULL;
    stack->size = 0;
    return stack;
}


// pop the top item of a stack
void* pop(Stack* stack) {
    if (stack->size == 0) {
        return NULL;
    }
    void* data = stack->head->val;
    StNode* tmp = stack->head;
    stack->head = stack->head->next;
    free(tmp);
    stack->size --;
    return data;
}


// Add a new item to the stack
void push(Stack* stack, void* data) {
    assert(data);
    if (stack->head == NULL) { // stack is NULL 
        stack->head = (StNode*) malloc(sizeof(StNode));
        assert(stack->head);
        stack->head->val = data;
        stack->head->next = NULL;
        stack->size ++;
        return;
    }
    StNode* tmp = (StNode*) malloc(sizeof(StNode));
    assert(tmp);
    tmp->val = data;
    tmp->next = stack->head;
    stack->head = tmp;
    stack->size ++;
}
