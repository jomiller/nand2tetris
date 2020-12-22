/*
 * This file is part of Nand2Tetris.
 *
 * Copyright © 2013-2020 Jonathan Miller
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

#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace n2t
{
enum class CommandType
{
    A,  // @Xxx
    C,  // dest=comp;jump
    L   // (Xxx)
};

class Parser
{
public:
    // Opens the input file and gets ready to parse it.
    explicit Parser(const std::filesystem::path& filename);

    // Returns the current line number.
    [[nodiscard]] unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    // Reads the next command from the input and makes it the current command.
    [[nodiscard]] bool advance();

    // Returns the type of the current command.
    [[nodiscard]] CommandType commandType() const;

    // Returns the symbol or decimal Xxx of the current command @Xxx or (Xxx).
    // Should be called only when commandType() is CommandType::A or CommandType::L.
    [[nodiscard]] const std::string& symbol() const;

    // Returns the 'dest' mnemonic in the current C-command (8 possibilities).
    // Should be called only when commandType() is CommandType::C.
    [[nodiscard]] const std::string& dest() const;

    // Returns the 'comp' mnemonic in the current C-command (28 possibilities).
    // Should be called only when commandType() is CommandType::C.
    [[nodiscard]] const std::string& comp() const;

    // Returns the 'jump' mnemonic in the current C-command (8 possibilities).
    // Should be called only when commandType() is CommandType::C.
    [[nodiscard]] const std::string& jump() const;

private:
    std::ifstream m_file;
    unsigned int  m_lineNumber  = 0;
    CommandType   m_commandType = CommandType::A;
    std::string   m_symbol;
    std::string   m_dest;
    std::string   m_comp;
    std::string   m_jump;
};
}  // namespace n2t

#endif
