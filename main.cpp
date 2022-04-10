#include <iostream>
#include <cmath>
#include <string>
#include "stack.h"

struct var {
    std::string name;
    int len;
    double value;
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
    if (current.empty())
        return NONE;
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
    return 0;
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
    return 0;
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
    for (int i = 0; i < example.size(); i++) {
        char symb = example[i];
        if (symb == ' ' || symb == '\t' || symb == '\n') {
            continue;
        }
//        PrintStack(numbers);
//        PrintStackChar(operations);
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
            i++;
            symb = example[i];
            std::string new_expr;
            int count_open = 1, count_closed = 1;
            while (symb != ')' || count_open != count_closed) {
                new_expr += symb;
                if (symb == '(')
                    count_open++;
                else if (symb == ')')
                    count_closed++;
                i++;
                symb = example[i];
            }
            double res = CountingExpression(new_expr, variable, count);
            if (current.empty()) {
                numbers = AddToStack(numbers, res);
            }
            else {
               if (current == "ln")
                   numbers = AddToStack(numbers, log(res));
               else if (current == "sqrt")
                   numbers = AddToStack(numbers, pow(res, 0.5));
               else if (current == "cos")
                   numbers = AddToStack(numbers, cos(res));
               else if (current == "sin")
                   numbers = AddToStack(numbers, sin(res));
               else if (current == "exp")
                   numbers = AddToStack(numbers, exp(res));
               //complex
//               else if (current == "ln")
//                   numbers = AddToStack(numbers, cos(res));
//               else if (current == "ln")
//                   numbers = AddToStack(numbers, cos(res));
//               else if (current == "ln")
//                   numbers = AddToStack(numbers, cos(res));
//               else if (current == "ln")
//                   numbers = AddToStack(numbers, cos(res));
                current = "";
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
    std::string a;
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


bool InVariables(std::string* names, std::string name) {
    int i = 0;
    while (!names[i].empty()) {
        if (name == names[i])
            return true;
        i++;
    }
    return false;
}

std::string* FindingVariables(std::string expr, int* count_of_variables) {
    std::string current;
    std::string* names = new std::string[1000];
    for (auto symb: expr) {
        if (symb == '\n' || symb == ' ' || symb == '\t' || symb == '(' || symb == ')' || symb == '*' || symb == '+' || symb == '-' ||
            symb == '/' || symb == '^' || symb == '=') {
            if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(names, current)) {
                names[(*count_of_variables)++] = current;
            }
            current = "";
        }
        else {
            current += symb;
        }
    }
    if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(names, current)) {
        names[(*count_of_variables)++] = current;
    }
    return names;
}


var* ReadingVariables(std::string* names, int &count) {
    auto* exprs = new std::string[1000];
    var* variables = new var[1000];
    int k = 0;
    while (k < count) {
        std::getline(std::cin, exprs[k]);
        int new_count = 0;
        std::string* new_names;
        new_names = FindingVariables(exprs[k], &new_count);
        for (int t = 0; t < new_count; t++) {
            if (!InVariables(names, new_names[t])) {
                names[count++] = new_names[t];
            }
        }
        delete[] new_names;
        k++;
    }
    for (int i = count - 1; i >= 0; i--) {
        std::string name;
        int j = 0;
        while (exprs[i][j] != '=') {
            name += exprs[i][j];
            j++;
        }
        int temp = j - 1;
        while (name[temp] == ' ' || name[temp] == '\t') {
            name.pop_back();
            temp--;
        }
        int s = 0;
        std::string send;

        // check later
        while (exprs[i][j] == ' ' || exprs[i][j] == '\t') {
            j++;
        }
        for (auto symb: exprs[i]) {
            if (s > j) {
                send += symb;
            }
            s++;
        }
        double result = CountingExpression(send, variables, count);
        variables[i] = {name, j, result};
    }
    delete[] exprs;
    return variables;
}


void Parsing() {
    char symb = '1';
    std::string expr;
    std::string current;
    auto* names = new std::string[1000];
    int count_of_variables = 0;
    while (symb != '\n') {
        symb = getchar();
        expr += symb;
        if (symb == '\n' || symb == ' ' || symb == '\t' || symb == '(' || symb == ')' || symb == '*' || symb == '+' || symb == '-' ||
        symb == '/' || symb == '^') {
            if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(names, current)) {
                names[count_of_variables++] = current;
            }
            current = "";
        }
        else {
            current += symb;
        }
    }
    var* variables;
    if (count_of_variables != 0) {
        variables = ReadingVariables(names, count_of_variables);
    }
    std::cout << CountingExpression(expr, variables, count_of_variables);
    delete[] names;
    delete[] variables;
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
