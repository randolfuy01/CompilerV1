#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <vector>
#include <optional>

#include "./tokenization.h"
#include "./parser.h"
#include "./generation.h"

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

    Parser parser(std::move(tokens));
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No exit statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());

    {
        std::fstream file("../out.asm", std::ios::out);
        file << generator.generate();
    }


    return EXIT_SUCCESS;
}
