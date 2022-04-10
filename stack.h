
#ifndef CALCULATOR_STACK_H
#define CALCULATOR_STACK_H

 struct stack {
    double* stack_array = nullptr;
    int size = 0;
    int reserve = 0;
    double head;
    bool is_empty = true;
};

stack* CreateStack();
stack* AddToStack(stack* pstack, double value);
double DeleteFromStack(stack* pstack);
void PrintStack(stack* pstack);
void PrintStackChar(stack* pstack);



#endif //CALCULATOR_STACK_H
