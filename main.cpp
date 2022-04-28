#include <iostream>
#include <cmath>
#include <string>
#include <complex>
#include "stack.h"

struct var {
    std::string name;
    int len;
    std::complex <double> value;
};


enum {NONE, NUMBER, VARIABLE, FUNCTION, COMPLEX};

std::string FUNCTIONS[9];


char priority[256];


char DoubleToChar(std::complex <double> x) {
    // convert double to char
    return (char)std::real(x);
}


//this
int TypeOfExpression(std::string current) {
    // define type of expression: number, variable or function/const
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
    flag = true;
    for (auto i: current) {
        if (!('0' <= i && i <= '9' || i == '.' || i == 'i')) {
            flag = false;
            break;
        }
    }
    if (flag) {
        return COMPLEX;
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


//this
std::complex <double> StringToNumber(std::string stroka) {
    // Convert string to number
    double x = 0;
    long long after_point = 10;
    bool flag_complex = false;
    bool point = false;
    for (auto i: stroka) {
        if (i == 'i') {
            flag_complex = true;
            break;
        }
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
    if (flag_complex) {
        if (x)
            return {0, x};
        return {0, 1};
    }
    return {x, 0};
}


std::complex <double> VariableValue(var* variable, std::string name, int count) {
    // Finds the variable in "variable"(array with type "var*"). Returns the value
    for (int i = 0; i < count; i++) {
        if (variable[i].name == name) {
            return variable[i].value;
        }
    }
    return 0;
}


std::complex <double> Counting(std::complex <double> b, std::complex <double> a, char operation) {
    // counting basic operations
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
        return std::pow(a, real(b));
    }
    return 0;
}

//  this
void ReloadStack(stack* &operations, stack* &numbers) {
    // This is a realisation of sort-station algorithm. Read here: https://habr.com/ru/post/50196/
    if (operations->size  < 2) {
        return;
    }


    char op1 = DoubleToChar(DeleteFromStack(operations)), op2 = DoubleToChar(DeleteFromStack(operations));
    // It's legacy. Now the calculator can work without this two "if"'s

    // counting
    if (priority[op1] <= priority[op2]) {
        std::complex <double> a = DeleteFromStack(numbers), b = DeleteFromStack(numbers);
        numbers = AddToStack(numbers, Counting(a, b, op2));
        operations = AddToStack(operations, op1);
        ReloadStack(operations, numbers);
    }
    // returns operators in stack in the another case
    else {
        operations = AddToStack(operations, op2);
        operations = AddToStack(operations, op1);
    }
}


std::complex <double> CountingExpression(std::string example, var* variable, int count) {
    // The most important function
    // Creates stacks: operations and numbers
    stack* operations;
    stack* numbers;
    operations = CreateStack();
    numbers = CreateStack();
    std::string current;
    // handles minus before the first number/variable/function
    if (example[0] == '-') {
        numbers = AddToStack(numbers, 0);
    }
    for (int i = 0; i < example.size(); i++) {
        char symb = example[i];
        if (symb == ' ' || symb == '\t' || symb == '\n') {
            continue;
        }
//        PrintStack(numbers);
//        PrintStackChar(operations);
//        Adds to stack new values after meet basic operations
        if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^' || symb == ')') {
            int result = TypeOfExpression(current);
            if (result == NUMBER || result == COMPLEX) {
                numbers = AddToStack(numbers, StringToNumber(current));
            }
            if (result == VARIABLE) {
                numbers = AddToStack(numbers, VariableValue(variable, current, count));
            }
            operations = AddToStack(operations, symb);
            ReloadStack(operations, numbers);
            current = "";
        }
        // separate processing of '(' and ')'. if the program meets '(' then it recursively calls "Counting Expression"
        // and counting the expression in the parentheses
        else if (symb == '(') {
            i++;
            symb = example[i];
            std::string new_expr;
            int count_open = 1, count_closed = 1;
            //checks if count of '(' is more than count of ')'
            while (symb != ')' || count_open != count_closed) {
                new_expr += symb;
                if (symb == '(')
                    count_open++;
                else if (symb == ')')
                    count_closed++;
                i++;
                symb = example[i];
            }
            std::complex <double> res = CountingExpression(new_expr, variable, count);
            if (current.empty()) {
                numbers = AddToStack(numbers, res);
            }
            // Handles functions(soon: constants)
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
               else if (current == "real")
                   numbers = AddToStack(numbers, std::real(res));
               else if (current == "imag")
                   numbers = AddToStack(numbers, std::imag(res));
               else if (current == "phase")
                   numbers = AddToStack(numbers, std::atan(std::imag(res) / std::real(res)));
               else if (current == "mag")
                   numbers = AddToStack(numbers, std::abs(res));
                current = "";
            }
        }
        else {
            current += symb;
        }
    }
    // processing after the cycle(if current is not empty)
    if (!current.empty()) {
        int result = TypeOfExpression(current);
        if (result == NUMBER || result == COMPLEX) {
            numbers = AddToStack(numbers, StringToNumber(current));
        }
        if (result == VARIABLE) {
            numbers = AddToStack(numbers, VariableValue(variable, current, count));
        }
    }
    //Counting while "operations" is not empty
    ReloadStack(operations, numbers);
    while (operations->size != 0) {
        std::complex <double> a = DeleteFromStack(numbers), b = DeleteFromStack(numbers);
        char op = DoubleToChar(DeleteFromStack(operations));
        numbers = AddToStack(numbers, Counting(a, b, op));
    }
    std::complex <double> result = numbers->head;
    //check memory
    delete numbers;
    delete operations;
    return result;
}


bool InVariables(std::string* names, std::string name) {
    //checks if the variable has already been met
    int i = 0;
    while (!names[i].empty()) {
        if (name == names[i])
            return true;
        i++;
    }
    return false;
}

std::string* FindingVariables(std::string expr, int* count_of_variables) {
    //Finds and remembers variables in math expression
    std::string current;
    std::string* names = new std::string[1000];
    for (auto symb: expr) {
        // finds different separators
        if (symb == '\n' || symb == ' ' || symb == '\t' || symb == '(' || symb == ')' || symb == '*' || symb == '+' || symb == '-' ||
            symb == '/' || symb == '^' || symb == '=') {
            // checks if "current" is a variable
            if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(names, current)) {
                names[(*count_of_variables)++] = current;
            }
            current = "";
        }
        else {
            current += symb;
        }
    }
    //checks after the cycle
    if (!current.empty() && TypeOfExpression(current) == VARIABLE && !InVariables(names, current)) {
        names[(*count_of_variables)++] = current;
    }
    return names;
}


var* ReadingVariables(std::string* names, int &count) {
    // reads variables in the following input strings and remembers it and its values(It starts to read in the last input string
    // and saves other strings in array "exprs"
    auto* exprs = new std::string[1000];
    var* variables = new var[1000];
    enum {e, pi};
    variables[e] = {"e", 1, M_E};
    variables[pi] = {"PI", 2, M_PI};
    int k = 0;
    while (k < count - 2) {
        std::getline(std::cin, exprs[k]);
        int new_count = 0;
        std::string* new_names;
        //check if there's new variables in the variable's value
        new_names = FindingVariables(exprs[k], &new_count);
        for (int t = 0; t < new_count; t++) {
            if (!InVariables(names, new_names[t])) {
                names[count++] = new_names[t];
            }
        }
        delete[] new_names;
        k++;
    }
    for (int i = count - 3; i >= 0; i--) {
        std::string name;
        int j = 0;
        // separate the name and the value
        while (exprs[i][j] != '=') {
            name += exprs[i][j];
            j++;
        }
        int temp = j - 1;
        // deletes extra spaces from the name
        while (name[temp] == ' ' || name[temp] == '\t') {
            name.pop_back();
            temp--;
        }
        int s = 0;
        std::string send;
        // increases "j" to get rid of extra spaces
        while (exprs[i][j] == ' ' || exprs[i][j] == '\t') {
            j++;
        }
        //counting the value
        for (auto symb: exprs[i]) {
            if (s > j) {
                send += symb;
            }
            s++;
        }
        std::complex <double> result = CountingExpression(send, variables, count);
        variables[i + 2] = {name, j, result};
    }
    delete[] exprs;
    return variables;
}


void Parsing() {
    char symb = '1';
    std::string expr;
    std::string current;
    auto* names = new std::string[1000];
    int count_of_variables = 2;
    enum {e, pi};
    names[e] = "e";
    names[pi] = "PI";
    //Finds variables in the original expression
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
    var* variables = nullptr;
    //reads the variables and saves it in array "variables"
    if (count_of_variables != 0) {
        variables = ReadingVariables(names, count_of_variables);
    }
    std::complex <double> result =  CountingExpression(expr, variables, count_of_variables);
    if (std::real(result) == INFINITY) {
        std::cout << "What the hell are you trying to do?";
    }
    else if (std::imag(result) != 0) {
        if (std::imag(result) > 0)
            std:: cout << std::real(result) << " + " << std::imag(result) << "i";
        else
            std:: cout << std::real(result) << " - " << std::imag(result) << "i";
    }
    else {
        std:: cout << std::real(result);
    }
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
