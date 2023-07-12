#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

// Function to evaluate binary mathematical expressions
double evaluateExpression(const std::string& expression, std::unordered_map<std::string, double>& variables) {
    std::stack<double> operandStack;
    std::stack<char> operatorStack;

    auto precedence = [](char op) {
        if (op == '*' || op == '/')
            return 2;
        else if (op == '+' || op == '-')
            return 1;
        else
            return 0;
    };

    auto applyOperator = [&]() {
        double operand2 = operandStack.top();
        operandStack.pop();
        double operand1 = operandStack.top();
        operandStack.pop();
        char oper = operatorStack.top();
        operatorStack.pop();
        double result;
        switch (oper) {
        case '+':
            result = operand1 + operand2;
            break;
        case '-':
            result = operand1 - operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case '/':
            if (operand2 == 0) {
                throw std::runtime_error("Error: Division by zero");
            }
            result = operand1 / operand2;
            break;
        default:
            throw std::runtime_error("Syntax error");
        }
        operandStack.push(result);
    };


    for (size_t i = 0; i < expression.length(); ++i) {
        if (isalpha(expression[i])) {
            std::string varName;
            while (i < expression.length() && (isalnum(expression[i]) || expression[i] == '_')) {
                varName.push_back(expression[i]);
                i++;
            }
            i--;
            if (variables.find(varName) != variables.end()) {
                operandStack.push(variables[varName]);
            }
            else {
                throw std::runtime_error("Error: Variable '" + varName + "' undefined");
            }
        }
        else if (isdigit(expression[i])) {
            double num = expression[i] - '0';
            double fraction = 1.0;
            while (i + 1 < expression.length() && (isdigit(expression[i + 1]) || expression[i + 1] == '.')) {
                i++;
                if (expression[i] == '.') {
                    fraction = 0.1;
                    continue;
                }
                if (fraction == 1.0) {
                    num = num * 10 + (expression[i] - '0');
                }
                else {
                    num = num + (expression[i] - '0') * fraction;
                    fraction /= 10.0;
                }
            }
            operandStack.push(num);
        }
        else if (expression[i] == '(') {
            operatorStack.push(expression[i]);
        }
        else if (expression[i] == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                applyOperator();
            }
            if (!operatorStack.empty() && operatorStack.top() == '(') {
                operatorStack.pop();
            }
            else {
                throw std::runtime_error("Syntax error");
            }
        }
        else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            if (i == 0 || (!isdigit(expression[i - 1]) && expression[i - 1] != ')' && expression[i - 1] != '_' && !isalpha(expression[i - 1]))) {
                // Handle negative numbers and variables
                bool isNegative = (expression[i] == '-');
                int num = 0;
                if (isNegative && isalpha(expression[i + 1])) {
                    std::string varName;
                    i++;
                    while (i < expression.length() && (isalnum(expression[i]) || expression[i] == '_')) {
                        varName.push_back(expression[i]);
                        i++;
                    }
                    i--;
                    if (variables.find(varName) != variables.end()) {
                        num = -variables[varName];
                    }
                    else {
                        throw std::runtime_error("Error: Variable '" + varName + "' undefined");
                    }
                }
                else {
                    while (i + 1 < expression.length() && isdigit(expression[i + 1])) {
                        i++;
                        num = num * 10 + (expression[i] - '0');
                    }
                    if (isNegative) {
                        num = -num;
                    }
                }
                operandStack.push(num);
            }
            else {
                while (!operatorStack.empty() && precedence(expression[i]) <= precedence(operatorStack.top())) {
                    applyOperator();
                }
                operatorStack.push(expression[i]);
            }
        }
        else if (expression[i] != ' ') {
            throw std::runtime_error("Syntax error");
        }
    }
    while (!operatorStack.empty()) {
        if (operatorStack.top() == '(' || operatorStack.top() == ')') {
            throw std::runtime_error("Syntax error");
        }
        applyOperator();
    }

    if (operandStack.empty()) {
        throw std::runtime_error("Syntax error");
    }

    return operandStack.top();
}

int main() {
    std::unordered_map<std::string, double> variables;
    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "Quit") {
            break;
        }

        try {
            size_t assignPos = input.find('=');
            if (assignPos != std::string::npos) {
                std::string varName = input.substr(0, assignPos);
                if (!isalpha(varName[0])) {
                    throw std::runtime_error("Error: Variable name must start with a letter");
                }
                std::string expression = input.substr(assignPos + 1);
                double result = evaluateExpression(expression, variables);
                variables[varName] = result;
                std::cout << varName << "=" << result << std::endl;
            }
            else {
                double result = evaluateExpression(input, variables);
                std::cout << result << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}