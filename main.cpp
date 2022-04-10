#include <iostream>
#include <cmath>
#include <string>
#include "stack.h"

struct var {
    std::string name;
    int len;
    int value;
};


enum {NONE, NUMBER, VARIABLE, FUNCTION};

std::string FUNCTIONS[9];


char priority[256];


char DoubleToChar(double x) {
    return (char)x;
}

double StringToNumber(std::string stroka) {
    double x = 0;
    long long after_point = 10;
    bool point = false;
    for (auto i: stroka) {
        if (i != '.') {
            if (!point) {
                x = x * 10 + i - '0';
            }
            else {
                x += ((double)(i - '0')) / after_point;
                if ((after_point * 10) / after_point == 10) {
                    after_point *= 10;
                }
            }
        }
        else {
            point = true;
        }
    }
    return x;
}

int TypeOfExpression(std::string current) {
    bool flag = true;
    for (auto i: current) {
        if (!('0' <= i && i <= '9' || i == '.')) {
            flag = false;
            break;
        }
    }
    if (flag) {
        return NUMBER;
    }
    for (std::string i: FUNCTIONS) {
       if (current == i) {
           flag = true;
           break;
       }
    }
    if (flag) {
        return FUNCTION;
    }
    return VARIABLE;

}

double VariableValue(var* variable, std::string name, int count) {
    for (int i = 0; i < count; i++) {
        if (variable[i].name == name) {
            return variable[i].value;
        }
    }
}


double Counting(double b, double a, char operation) {
    if (operation == '+') {
        return a + b;
    }
    if (operation == '-') {
        return a - b;
    }
    if (operation == '*') {
        return a * b;
    }
    if (operation == '/') {
        return a / b;
    }
    if (operation == '^') {
        return std::pow(a, b);
    }
}


void ReloadStack(stack* &operations, stack* &numbers) {
//    PrintStackChar(operations);
//    PrintStack(numbers);
//    std::cout << std::endl;
    if (operations->size  < 2) {
        return;
    }
    char op1 = DoubleToChar(DeleteFromStack(operations)), op2 = DoubleToChar(DeleteFromStack(operations));
    if (op1 == '(' || op2 == '(') {
        operations = AddToStack(operations, op2);
        operations = AddToStack(operations, op1);
        return;
    }
    if (op1 == ')') {
        while (op2 != '(') {
            double a = DeleteFromStack(numbers), b = DeleteFromStack(numbers);
            numbers = AddToStack(numbers, Counting(a, b, op2));
            op2 = DoubleToChar(DeleteFromStack(operations));
        }
    }
    else if (priority[op1] <= priority[op2]) {
        double a = DeleteFromStack(numbers), b = DeleteFromStack(numbers);
        numbers = AddToStack(numbers, Counting(a, b, op2));
        operations = AddToStack(operations, op1);
        ReloadStack(operations, numbers);
    }
    else {
        operations = AddToStack(operations, op2);
        operations = AddToStack(operations, op1);
    }
}


double CountingExpression(std::string example, var* variable, int count) {
    stack* operations;
    stack* numbers;
    operations = CreateStack();
    numbers = CreateStack();
    std::string current;
    char predop;
    for (char symb: example) {
        if (symb == ' ' || symb == '\t' || symb == '\n') {
            continue;
        }
        if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^' || symb == ')') {
            predop = symb;
            int result = TypeOfExpression(current);
            if (result == NUMBER) {
                numbers = AddToStack(numbers, StringToNumber(current));
            }
            if (result == VARIABLE) {
                numbers = AddToStack(numbers, VariableValue(variable, current, count));
            }
            operations = AddToStack(operations, symb);
            ReloadStack(operations, numbers);
            current = "";
        }
        else if (symb == '(') {
            if (current.empty() && predop != '^') {
                operations = AddToStack(operations, '(');
            }
            else if (predop == '^'){
                // here will be written the processing of variables//
            }
        }
        else {
            current += symb;
        }
    }
    if (!current.empty()) {
        int result = TypeOfExpression(current);
        if (result == NUMBER) {
            numbers = AddToStack(numbers, StringToNumber(current));
        }
        if (result == VARIABLE) {
            numbers = AddToStack(numbers, VariableValue(variable, current, count));
        }
    }
    ReloadStack(operations, numbers);
    while (operations->size != 0) {
        double a = DeleteFromStack(numbers), b = DeleteFromStack(numbers);
        char op = DoubleToChar(DeleteFromStack(operations));
        numbers = AddToStack(numbers, Counting(a, b, op));
    }
    double result = numbers->head;
    //check memory
    delete numbers;
    delete operations;
    return result;
}


void ReadingVariables(var* variables, int count) {
    auto* exprs = new std::string[count];
    for (int i = 0; i < count; i++) {
        std::getline(std::cin, exprs[i]);
    }
    for (int i = count - 1; i > 0; i--) {
        std::string name;
        int j = 0;
        while (exprs[i][j] != '=') {
            name += exprs[i][j];
        }
        int k = 0;
        std::string send;

        // check later

        for (auto symb: exprs[i]) {
            k++;
            if (k > j) {
                send += symb;
            }
        }
        if (name[j - 1] == ' ') {
            name.pop_back();
            j--;
        }

        int result = CountingExpression(send, variables, count);
        variables[i] = {name, j, result};
    }
}

bool InVariables(var* variables, std::string name) {
    int i = 0;
    while (!variables[i].name.empty()) {
        if (name == variables[i].name)
            return true;
        i++;
    }
    return false;
}


void Parsing() {
    char symb = '1';
    std::string expr;
    std::string current;
    var* variables = new var[1000];
    int count_of_variables = 0;
    while (symb != '\n') {
        symb = getchar();
        expr += symb;
        if (symb == '\n' || symb == ' ' || symb == '\t' || symb == '(' || symb == ')' || symb == '*' || symb == '+' || symb == '-' ||
        symb == '/' || symb == '^') {
            if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(variables, current)) {
                variables[count_of_variables++].name = current;
            }
            current = "";
        }
        else {
            current += symb;
        }
    }
    if (count_of_variables != 0) {
        ReadingVariables(variables, count_of_variables);
    }
    std::cout << CountingExpression(expr, variables, count_of_variables);
}


int main() {
    FUNCTIONS[0] = "sin";
    FUNCTIONS[1] = "cos";
    FUNCTIONS[2] = "ln";
    FUNCTIONS[3] = "sqrt";
    FUNCTIONS[4] = "exp";
    FUNCTIONS[5] = "real";
    FUNCTIONS[6] = "imag";
    FUNCTIONS[7] = "mag";
    FUNCTIONS[8] = "phase";
    priority['+'] = 1;
    priority['-'] = 1;
    priority['*'] = 2;
    priority['/'] = 2;
    priority['^'] = 3;
    Parsing();
    return 0;
}
