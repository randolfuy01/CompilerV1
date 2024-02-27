#pragma once
#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <utility>
#include <vector>
#include <optional>
#include <algorithm>

enum class TokenType {
    exit,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
public:
    inline explicit Tokenizer(std::string  src)
        : m_src(std::move(src))
    {

    }

    inline std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buffer;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buffer.push_back(consume());
                }
                if (buffer == "exit") {
                    tokens.push_back({ .type = TokenType::exit });
                    buffer.clear();
                } else {
                    std::cerr << "Unhandled identifier: " << buffer << std::endl;
                }
            } else if (std::isdigit(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buffer.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .value = buffer });
                buffer.clear();
                continue;
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({ .type = TokenType::semi });
                continue;
            } else if (std::isspace(peek().value())) {
                consume();
                continue;
            } else {
                std::cerr << "Unhandled identifier: " << buffer << std::endl;
            }
        }
        m_index = 0;
        return tokens;
    }

private:

    [[nodiscard]] inline std::optional<char> peek(int ahead = 1) const {
        if (m_index + ahead > m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    int m_index = 0;
};
