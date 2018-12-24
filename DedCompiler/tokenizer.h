#pragma once

#include <cctype>
#include <iostream>
#include <variant>

struct ConstantToken {
    int number;
};

bool operator==(const ConstantToken& lhs, const ConstantToken& rhs) {
    return lhs.number == rhs.number;
}

struct OperationToken {
    char type;
};

bool operator==(const OperationToken& lhs, const OperationToken& rhs) {
    return lhs.type == rhs.type;
}

enum class BracketToken { OPEN, CLOSE };

struct SymbolToken {
    std::string symbol;
};

bool operator==(const SymbolToken& lhs, const SymbolToken& rhs) {
    return lhs.symbol == rhs.symbol;
}

typedef std::variant<ConstantToken, OperationToken, BracketToken, SymbolToken> Token;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : in_(in) {
    }

    bool IsEnd() {
        SkipSpaces();
        return in_->peek() == EOF;
    }

    void Next() {
        SkipSpaces();

        if (in_->peek() == EOF) {
            return;
        }

        char current = static_cast<char>(in_->peek());

        if (current == '(' || current == ')' || current == '+' || current == '-' ||
            current == '*' || current == '/') {
            in_->get();
            return;
        }

        if (std::isdigit(current)) {
            while (std::isdigit(in_->peek())) {
                in_->get();
            }
            return;
        }

        while (std::isalpha(in_->peek())) {
            in_->get();
        }
    }

    Token GetToken() {
        SkipSpaces();
        char current = static_cast<char>(in_->peek());

        if (current == '(') {
            return Token{BracketToken::OPEN};
        }
        if (current == ')') {
            return Token{BracketToken::CLOSE};
        }

        if (std::isdigit(current)) {
            auto pos = in_->tellg();
            int number = 0;
            while (std::isdigit(in_->peek())) {
                number *= 10;
                number += in_->peek() - static_cast<int>('0');
                in_->get();
            }
            in_->seekg(pos);
            return Token{ConstantToken{number}};
        }

        if (current == '+' || current == '-' || current == '*' || current == '/') {
            return Token{OperationToken{current}};
        }

        auto pos = in_->tellg();
        std::string symbol;
        while (std::isalpha(in_->peek())) {
            symbol += static_cast<char>(in_->peek());
            in_->get();
        }
        in_->seekg(pos);
        return Token{SymbolToken{symbol}};
    }

private:
    void SkipSpaces() {
        while (in_->peek() != EOF && std::isspace(in_->peek())) {
            in_->get();
        }
    }

    std::istream* in_;
};
