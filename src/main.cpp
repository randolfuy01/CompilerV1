#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <vector>
#include <optional>

enum class TokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;
    std::string buffer;
    for (size_t i = 0; i < str.length(); ++i) { // Use size_t for string length
        char c = str[i]; // Direct access is fine here
        if (std::isalpha(c)) {
            buffer.push_back(c);
            while (i + 1 < str.length() && std::isalnum(str[i + 1])) { // Check bounds
                buffer.push_back(str[++i]);
            }

            if (buffer == "return") {
                tokens.push_back(Token{TokenType::_return, std::nullopt}); // Use std::nullopt for optional without value
                buffer.clear();
            } else {
                std::cerr << "Unhandled identifier: " << buffer << std::endl;
                // Handle error more gracefully, potentially throw an exception
            }
        }
        else if (std::isdigit(c)) {
            buffer.push_back(c);
            while (i + 1 < str.length() && std::isdigit(str[i + 1])) { // Check bounds
                buffer.push_back(str[++i]);
            }
            tokens.push_back(Token{TokenType::int_lit, buffer});
            buffer.clear();
        }
        else if (c == ';') {
            tokens.push_back(Token{TokenType::semi, std::nullopt}); // Use std::nullopt
        }
            // Skipping space is already handled implicitly by the loop's continuation condition
        else if (!std::isspace(c)) {
            std::cerr << "Unsupported character: " << c << std::endl;
            // Handle error more gracefully, potentially throw an exception
        }
    }

    return tokens;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n"; // Changed start to _start for consistency with global declaration
    for (size_t i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens[i + 1].value.value_or("0") << "\n"; // Safer access with value_or
                    output << "    syscall\n"; // Added newline for formatting
                    i += 2; // Skip the next two tokens since they're part of this instruction
                }
            }
        }
        // Handling for other token types can be added here
    }
    return output.str(); // Return the assembled output
}

// The count of how many arguments and an array of strings
// The first argument is always the executable itself
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "lazy <input.lz>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    std::vector<Token> tokens = tokenize(contents);
    std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("../out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    return EXIT_SUCCESS;
}
