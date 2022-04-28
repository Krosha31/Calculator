#include "stack.h"
#include <iostream>


stack* CreateStack() {
    stack* new_stack = new stack;
    new_stack->stack_array = new std::complex <double>[100];
    new_stack->reserve = 10;
    return new_stack;
}


stack* WideningStack(stack* pstack) {
    stack* new_stack = new stack;
    new_stack->stack_array = new std::complex <double>[pstack->reserve * 2];
    new_stack->reserve = pstack->reserve * 2;
    for (int i = 0; i < pstack->reserve; i++) {
        new_stack->stack_array[i] = pstack->stack_array[i];
    }
    delete pstack;
    return new_stack;
}


stack* AddToStack(stack* pstack, std::complex <double> value) {
    if (pstack->reserve == pstack->size) {
        pstack = WideningStack(pstack);
    }
    pstack->stack_array[pstack->size++] = value;
    pstack->head = value;
    pstack->is_empty = false;
    return pstack;
}

std::complex <double> DeleteFromStack(stack* pstack) {
    std::complex <double> temp = pstack->head;
    if (!pstack->is_empty) {
        pstack->size--;
        pstack->head = pstack->stack_array[pstack->size - 1];
    }
    if (!pstack->size)
        pstack->is_empty = true;
    return temp;
}


void PrintStack(stack* pstack) {
    for (int i = 0; i < pstack->size; i++) {
        std::cout << pstack->stack_array[i] << " ";
    }
    std::cout << std::endl;
}


void PrintStackChar(stack* pstack) {
    for (int i = 0; i < pstack->size; i++) {
        std::cout << (char)real(pstack->stack_array[i]) << " ";
    }
    std::cout << std::endl;
}


