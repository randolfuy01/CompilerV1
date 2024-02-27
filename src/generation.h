#pragma once

#include "parser.h"

class Generator {
public:
    inline explicit Generator(NodeExit root)
        : m_root(std::move(root))
    {
    }

    [[nodiscard]] std::string generate() const {
        std::stringstream output;
        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n";
        output << "    mov rdi, " << m_root.expr.int_lit.value.value() << "\n"; // Safer access with value_or
        output << "    syscall\n";
        return output.str();
    }
private:
    const NodeExit m_root;
};