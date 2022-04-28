
#ifndef CALCULATOR_STACK_H
#define CALCULATOR_STACK_H

#include <iostream>

#include <complex>

 struct stack {
    std::complex <double>* stack_array = nullptr;
    int size = 0;
    int reserve = 0;
    std::complex <double> head;
    bool is_empty = true;
};

stack* CreateStack();
stack* AddToStack(stack* pstack, std::complex <double> value);
std::complex <double> DeleteFromStack(stack* pstack);
void PrintStack(stack* pstack);
void PrintStackChar(stack* pstack);



#endif //CALCULATOR_STACK_H
