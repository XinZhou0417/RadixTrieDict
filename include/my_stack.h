/*-------------------------------------------------------------- 
Created by Xin Zhou (xzzho10@student.unimelb.edu.au)
07 Sep 2023
..Project: Assignment2
my_stack.h :
        = Interface of Stack
----------------------------------------------------------------*/


#ifndef _MY_STACK_H_
#define _MY_STACK_H_
#include <stdio.h>

typedef struct MyStack Stack;

// get a new stack
Stack* newStack();

// pop the top item of a stack
void* pop(Stack* stack);

// Add a new item to the stack
void push(Stack* stack, void* data);

// Get the size of a stack  
size_t getStackSize(Stack* stack);

#endif