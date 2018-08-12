/*
 * This file is part of Nand2Tetris.
 *
 * Copyright (c) 2013-2018 Jonathan Miller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ASSEMBLY_ENGINE_H
#define ASSEMBLY_ENGINE_H

#include "SymbolTable.h"

#include <filesystem>
#include <string>

namespace n2t
{
class AssemblyEngine
{
public:
    AssemblyEngine(std::filesystem::path inputFilename, std::filesystem::path outputFilename);

    AssemblyEngine(const AssemblyEngine&) = delete;
    AssemblyEngine(AssemblyEngine&&)      = delete;

    AssemblyEngine& operator=(const AssemblyEngine&) = delete;
    AssemblyEngine& operator=(AssemblyEngine&&) = delete;

    ~AssemblyEngine() noexcept;

    void assemble();

private:
    // Builds the symbol table without generating any code.
    void buildSymbolTable();

    // Replaces each symbol with its corresponding meaning (numeric address)
    // and generates the final binary code.
    void generateCode();

    std::filesystem::path m_inputFilename;
    std::filesystem::path m_outputFilename;
    SymbolTable           m_symbolTable;
    bool                  m_assembled = false;
};
}  // namespace n2t

#endif
