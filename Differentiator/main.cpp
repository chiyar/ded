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
    LOG,
    SIN,
    COS
};

std::unordered_map<Operation, std::string> operation_to_string = {
    {ADD, "+"},
    {SUB, "-"},
    {MUL, "*"},
    {DIV, "/"},
    {LOG, "log"},
    {SIN, "sin"},
    {COS, "cos"}
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

auto Log(const std::shared_ptr<Expression>& expr) {
    return std::make_shared<Expression>(LOG, expr, nullptr);
}

auto Sin(const std::shared_ptr<Expression>& expr) {
    return std::make_shared<Expression>(SIN, expr, nullptr);
}

auto Cos(const std::shared_ptr<Expression>& expr) {
    return std::make_shared<Expression>(COS, expr, nullptr);
}

std::shared_ptr<Expression> Differentiate(const std::shared_ptr<Expression>& expr,
                                          const Variable& dx);

auto DifferentiateAdd(const std::shared_ptr<Expression>& lhs,
                      const std::shared_ptr<Expression>& rhs,
                      const Variable& dx) {
    return Differentiate(lhs, dx) + Differentiate(rhs, dx);
}

auto DifferentiateSub(const std::shared_ptr<Expression>& lhs,
                      const std::shared_ptr<Expression>& rhs,
                      const Variable& dx) {
    return Differentiate(lhs, dx) - Differentiate(rhs, dx);
}

auto DifferentiateMul(const std::shared_ptr<Expression>& lhs,
                      const std::shared_ptr<Expression>& rhs,
                      const Variable& dx) {
    return Differentiate(lhs, dx) * rhs + lhs * Differentiate(rhs, dx);
}

auto DifferentiateDiv(const std::shared_ptr<Expression>& lhs,
                      const std::shared_ptr<Expression>& rhs,
                      const Variable& dx) {
    return (Differentiate(lhs, dx) * rhs - lhs * Differentiate(rhs, dx)) / (rhs * rhs);
}

auto DifferentiateLog(const std::shared_ptr<Expression>& expr, const Variable& dx) {
    return (std::make_shared<Expression>(Constant{1.}) / expr) * Differentiate(expr, dx);
}

auto DifferentiateSin(const std::shared_ptr<Expression>& expr, const Variable& dx) {
    return Cos(expr) * Differentiate(expr, dx);
}

auto DifferentiateCos(const std::shared_ptr<Expression>& expr, const Variable& dx) {
    return std::make_shared<Expression>(Constant{-1.}) * Sin(expr) * Differentiate(expr, dx);
}

auto DifferentiateConstant(const std::shared_ptr<Expression>& expr) {
    return std::make_shared<Expression>(Constant{0.});
}

auto DifferentiateVariable(const std::shared_ptr<Expression>& expr, const Variable& dx) {
    return std::make_shared<Expression>(
        std::get<Variable>(expr->expression_).name == dx.name ? Constant{1.} : Constant{0.});
}

std::shared_ptr<Expression> Differentiate(const std::shared_ptr<Expression>& expr,
                                          const Variable& dx) {
    int type = expr->expression_.index();
    if (type == 0) {  // Operation
        Operation operation = std::get<Operation>(expr->expression_);
        switch (operation) {
            case ADD:return DifferentiateAdd(expr->left_, expr->right_, dx);
            case SUB:return DifferentiateSub(expr->left_, expr->right_, dx);
            case MUL:return DifferentiateMul(expr->left_, expr->right_, dx);
            case DIV:return DifferentiateDiv(expr->left_, expr->right_, dx);
            case LOG:return DifferentiateLog(expr->left_, dx);
            case SIN:return DifferentiateSin(expr->left_, dx);
            case COS:return DifferentiateCos(expr->left_, dx);

        }
    } else if (type == 1) {  // Constant
        return DifferentiateConstant(expr);
    } else if (type == 2) {  // Variable
        return DifferentiateVariable(expr, dx);
    }
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
        case ADD:
            result = left + right;
            break;
        case SUB:
            result = left - right;
            break;
        case MUL:
            result = left * right;
            break;
        case DIV:
            result = left / right;
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



void ExpressionToString(const std::shared_ptr<Expression>& expr, std::ostream* out);

void BinaryOperationToString(const std::shared_ptr<Expression>& expr, std::ostream* out) {
    *out << "(";
    ExpressionToString(expr->left_, out);

    Operation operation = std::get<Operation>(expr->expression_);
    *out << operation_to_string[operation];

    ExpressionToString(expr->right_, out);
    *out << ")";
}

void UnaryOperationToString(const std::shared_ptr<Expression>& expr, std::ostream* out) {
    Operation operation = std::get<Operation>(expr->expression_);

    *out << operation_to_string[operation] << "(";
    ExpressionToString(expr->left_, out);
    *out << ")";
}

void ConstantToString(Constant constant, std::ostream* out) {
    if (constant.value >= 0.) {
        *out << constant.value;
    } else {
        *out << "(" << constant.value << ")";
    }
}

void VariableToString(Variable variable, std::ostream* out) {
    *out << variable.name;
}

void ExpressionToString(const std::shared_ptr<Expression>& expr, std::ostream* out) {
    int type = expr->expression_.index();
    if (type == 0) {  // Operation
        Operation operation = std::get<Operation>(expr->expression_);
        if (IsBinaryOperation(operation)) {
            BinaryOperationToString(expr, out);
        } else {
            UnaryOperationToString(expr, out);
        }
    } else if (type == 1) {  // Constant
        Constant constant = std::get<Constant>(expr->expression_);
        ConstantToString(constant, out);
    } else if (type == 2) {  // Variable
        Variable variable = std::get<Variable>(expr->expression_);
        VariableToString(variable, out);
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
    } else if (token == Token{BracketToken::CLOSE}) {
        std::cout << "Parse error: unexpected ')'" << "\n";
        exit(0);
    } else if (token.index() == 1) {
        std::cout << "Parse error: unexpected '" << std::get<OperationToken>(token).type << "'\n";
        exit(0);
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
        if (name == "log") {
            operation = LOG;
        } else if (name == "sin") {
            operation = SIN;
        } else if (name == "cos") {
            operation = COS;
        } else {
            is_variable = true;
            variable.name = name;
        }

        if (is_variable) {
            return std::make_shared<Expression>(variable);
        } else {
            auto result = std::make_shared<Expression>(operation, StringToExpression(tokenizer), nullptr);
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

int main() {
    std::cout << "Enter expression:\n";
    std::string str;
    std::getline(std::cin, str);
    std::stringstream in{str};

    Tokenizer tokenizer(&in);
    auto expr = StringToExpression(&tokenizer);

    std::cout << "Enter variable name:\n";
    Variable variable;
    std::cin >> variable.name;

    auto d_expr = Differentiate(expr, variable);
    SimplifyExpression(d_expr);
    std::stringstream out;
    ExpressionToString(d_expr, &out);
    std::cout << "Result: " << out.rdbuf() << "\n";

    return 0;
}
