#include <iostream>
#include <memory>
#include <variant>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "tokenizer.h"

enum Operation {
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT
};

std::unordered_map<Operation, std::string> operation_to_string = {
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MUL, "MUL"},
    {DIV, "DIV"},
    {SQRT, "SQRT"}
};

std::unordered_set<Operation> binary_operations = {
    ADD,
    SUB,
    MUL,
    DIV
};

bool IsBinaryOperation(Operation operation) {
    return binary_operations.find(operation) != binary_operations.end();
}

struct Constant {
    double value;
};

struct Variable {
    std::string name;
};

using ExpressionType = std::variant<Operation, Constant, Variable>;

class Expression {
public:
    Expression(Operation operation,
               const std::shared_ptr<Expression>& left,
               const std::shared_ptr<Expression>& right)
        : left_(left), right_(right), expression_(operation) {
    }

    explicit Expression(Constant constant)
        : left_(nullptr), right_(nullptr), expression_(constant) {
    }

    explicit Expression(Variable variable)
        : left_(nullptr), right_(nullptr), expression_(variable) {
    }

    Expression(const Expression& other)
        : left_(other.left_), right_(other.right_), expression_(other.expression_) {
    }

    Expression& operator=(const Expression& other) {
        left_ = other.left_;
        right_ = other.right_;
        expression_ = other.expression_;
        return *this;
    }

    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    ExpressionType expression_;
};

auto operator+(const std::shared_ptr<Expression>& lhs, const std::shared_ptr<Expression>& rhs) {
    return std::make_shared<Expression>(ADD, lhs, rhs);
}

auto operator-(const std::shared_ptr<Expression>& lhs, const std::shared_ptr<Expression>& rhs) {
    return std::make_shared<Expression>(SUB, lhs, rhs);
}

auto operator*(const std::shared_ptr<Expression>& lhs, const std::shared_ptr<Expression>& rhs) {
    return std::make_shared<Expression>(MUL, lhs, rhs);
}

auto operator/(const std::shared_ptr<Expression>& lhs, const std::shared_ptr<Expression>& rhs) {
    return std::make_shared<Expression>(DIV, lhs, rhs);
}

auto Sqrt(const std::shared_ptr<Expression>& expr) {
    return std::make_shared<Expression>(SQRT, expr, nullptr);
}



void SimplifyMulByZero(std::shared_ptr<Expression>& expr) {
    expr = std::make_shared<Expression>(Constant{0.});
}

void SimplifyMulByOne(std::shared_ptr<Expression>& expr) {
    if (expr->left_->expression_.index() == 1) {
        auto number = std::get<Constant>(expr->left_->expression_).value;
        if (number == 1.) {
            expr = expr->right_;
        } else {
            expr = expr->left_;
        }
    } else {
        auto number = std::get<Constant>(expr->right_->expression_).value;
        if (number == 1.) {
            expr = expr->left_;
        } else {
            expr = expr->right_;
        }
    }
}

void SimplifyDivByOne(std::shared_ptr<Expression>& expr) {
    if (expr->right_->expression_.index() == 1) {
        auto number = std::get<Constant>(expr->right_->expression_).value;
        if (number == 1.) {
            expr = expr->left_;
        } else {
            expr = expr->right_;
        }
    }
}

void SimplifyAddWithZero(std::shared_ptr<Expression>& expr) {
    if (expr->left_->expression_.index() == 1) {
        auto number = std::get<Constant>(expr->left_->expression_).value;
        if (number == 0.) {
            expr = expr->right_;
        } else {
            expr = expr->left_;
        }
    } else {
        auto number = std::get<Constant>(expr->right_->expression_).value;
        if (number == 0.) {
            expr = expr->left_;
        } else {
            expr = expr->right_;
        }
    }
}

void SimplifyOperationWithConstants(std::shared_ptr<Expression>& expr) {
    Operation operation = std::get<Operation>(expr->expression_);
    double left = std::get<Constant>(expr->left_->expression_).value;
    double right = std::get<Constant>(expr->right_->expression_).value;
    double result = 0;
    switch (operation) {
        case ADD:result = left + right;
            break;
        case SUB:result = left - right;
            break;
        case MUL:result = left * right;
            break;
        case DIV:result = left / right;
            break;
    }
    expr = std::make_shared<Expression>(Constant{result});
}

bool Simplify(std::shared_ptr<Expression>& expr) {
    bool flag = false;
    if (expr->left_ != nullptr) {
        flag |= Simplify(expr->left_);
    }
    if (expr->right_ != nullptr) {
        flag |= Simplify(expr->right_);
    }

    if (expr->left_ != nullptr && expr->right_ != nullptr) {
        if (expr->left_->expression_.index() == 1 && expr->right_->expression_.index() == 1) {
            SimplifyOperationWithConstants(expr);
            return true;
        }

        Operation operation = std::get<Operation>(expr->expression_);
        const double INF = 1e18;
        double left = INF;
        double right = INF;
        if (expr->left_->expression_.index() == 1) {
            left = std::get<Constant>(expr->left_->expression_).value;
        }
        if (expr->right_->expression_.index() == 1) {
            right = std::get<Constant>(expr->right_->expression_).value;
        }
        if (operation == MUL) {
            if (left == 1. || right == 1.) {
                SimplifyMulByOne(expr);
                return true;
            }

            if (left == 0. || right == 0.) {
                SimplifyMulByZero(expr);
                return true;
            }
        } else if (operation == DIV) {
            if (right == 1.) {
                SimplifyDivByOne(expr);
                return true;
            }
        } else if (operation == ADD) {
            if (left == 0. || right == 0) {
                SimplifyAddWithZero(expr);
                return true;
            }
        }
    }
    return flag;
}

void SimplifyExpression(std::shared_ptr<Expression>& expr) {
    while (Simplify(expr)) {}
}




void ExpressionToAsm(const std::shared_ptr<Expression>& expr,
                     const std::unordered_map<std::string, size_t>& var_index_in_memory,
                     std::ostream* out);

void BinaryOperationToAsm(const std::shared_ptr<Expression>& expr,
                          const std::unordered_map<std::string, size_t>& var_index_in_memory,
                          std::ostream* out) {
    ExpressionToAsm(expr->left_, var_index_in_memory, out);
    ExpressionToAsm(expr->right_, var_index_in_memory, out);

    Operation operation = std::get<Operation>(expr->expression_);
    *out << operation_to_string[operation] << "\n";
}

void UnaryOperationToAsm(const std::shared_ptr<Expression>& expr,
                         const std::unordered_map<std::string, size_t>& var_index_in_memory,
                         std::ostream* out) {
    ExpressionToAsm(expr->left_, var_index_in_memory, out);
    Operation operation = std::get<Operation>(expr->expression_);
    *out << operation_to_string[operation] << "\n";
}

void ConstantToAsm(Constant constant,
                   const std::unordered_map<std::string, size_t>& var_index_in_memory,
                   std::ostream* out) {
    *out << "PUSH " << constant.value << "\n";
}

void VariableToAsm(Variable variable,
                   const std::unordered_map<std::string, size_t>& var_index_in_memory,
                   std::ostream* out) {
    auto found = var_index_in_memory.find(variable.name);
    if (found == var_index_in_memory.end()) {
        std::cout << "Undefined variable in expression: " << variable.name << "\n";
        exit(0);
    }

    *out << "MOV_MEMTOS " << found->second << "\n";
}

void ExpressionToAsm(const std::shared_ptr<Expression>& expr,
                     const std::unordered_map<std::string, size_t>& var_index_in_memory,
                     std::ostream* out) {
    int type = expr->expression_.index();
    if (type == 0) {  // Operation
        Operation operation = std::get<Operation>(expr->expression_);
        if (IsBinaryOperation(operation)) {
            BinaryOperationToAsm(expr, var_index_in_memory, out);
        } else {
            UnaryOperationToAsm(expr, var_index_in_memory, out);
        }
    } else if (type == 1) {  // Constant
        Constant constant = std::get<Constant>(expr->expression_);
        ConstantToAsm(constant, var_index_in_memory, out);
    } else if (type == 2) {  // Variable
        Variable variable = std::get<Variable>(expr->expression_);
        VariableToAsm(variable, var_index_in_memory, out);
    }
}





std::shared_ptr<Expression> StringToExpression(Tokenizer* tokenizer);

std::shared_ptr<Expression> FactorToExpression(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    if (token == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        auto expr = StringToExpression(tokenizer);
        tokenizer->Next();
        return expr;
    } else if (token.index() == 0) {  // ConstantToken
        tokenizer->Next();
        auto constant = std::get<ConstantToken>(token);
        auto value = static_cast<double>(constant.number);
        return std::make_shared<Expression>(Constant{value});
    } else if (token.index() == 3) {  // SymbolToken
        tokenizer->Next();
        auto name = std::get<SymbolToken>(token).symbol;

        bool is_variable = false;
        Operation operation;
        Variable variable;
        if (name == "sqrt") {
            operation = SQRT;
        } else {
            is_variable = true;
            variable.name = name;
        }

        if (is_variable) {
            return std::make_shared<Expression>(variable);
        } else {
            auto result =
                std::make_shared<Expression>(operation, StringToExpression(tokenizer), nullptr);
            tokenizer->Next();
            return result;
        }
    }
}

std::shared_ptr<Expression> SummandToExpression(Tokenizer* tokenizer) {
    auto factor = FactorToExpression(tokenizer);
    while (!tokenizer->IsEnd() && (tokenizer->GetToken() == Token{OperationToken{'*'}} ||
        tokenizer->GetToken() == Token{OperationToken{'/'}})) {
        Operation operation;
        if (tokenizer->GetToken() == Token{OperationToken{'*'}}) {
            operation = MUL;
        } else {
            operation = DIV;
        }

        tokenizer->Next();
        if (operation == MUL) {
            factor = factor * FactorToExpression(tokenizer);
        } else {
            factor = factor / FactorToExpression(tokenizer);
        }
    }
    return factor;
}

std::shared_ptr<Expression> StringToExpression(Tokenizer* tokenizer) {
    while (!tokenizer->IsEnd() && tokenizer->GetToken() == Token{BracketToken::OPEN}) {
        tokenizer->Next();
    }
    auto summand = SummandToExpression(tokenizer);
    while (!tokenizer->IsEnd() && (tokenizer->GetToken() == Token{OperationToken{'+'}} ||
        tokenizer->GetToken() == Token{OperationToken{'-'}})) {
        Operation operation;
        if (tokenizer->GetToken() == Token{OperationToken{'+'}}) {
            operation = ADD;
        } else {
            operation = SUB;
        }

        tokenizer->Next();
        if (operation == ADD) {
            summand = summand + SummandToExpression(tokenizer);
        } else {
            summand = summand - SummandToExpression(tokenizer);
        }
    }
    return summand;
}
