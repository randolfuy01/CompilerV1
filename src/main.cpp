#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <vector>
#include <optional>

#include "./tokenization.h"
#include "./parser.h"

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n"; // Changed start to _start for consistency with global declaration
    for (size_t i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::exit) {
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

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();
    std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("../out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }


    return EXIT_SUCCESS;
}
