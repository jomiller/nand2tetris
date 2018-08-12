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

#ifndef PARSER_H
#define PARSER_H

#include "VmTypes.h"

#include <cstdint>
#include <fstream>
#include <string>

namespace n2t
{
class Parser
{
public:
    // Opens the input file and gets ready to parse it.
    explicit Parser(const std::string& filename);

    // Returns the current line number.
    unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    // Reads the next command from the input and makes it the current command.
    bool advance();

    // Returns the type of the current VM command.
    // Arithmetic is returned for all the arithmetic commands.
    CommandType commandType() const;

    // Returns the first argument of the current command.
    // In the case of Arithmetic, the command itself (add, sub, etc.) is returned.
    // Should not be called if the current command is Return
    const std::string& arg1() const;

    // Returns the second argument of the current command.
    // Should be called only if the current command is Push, Pop, Function, or Call.
    int16_t arg2() const;

private:
    std::ifstream m_file;
    unsigned int  m_lineNumber  = 0;
    CommandType   m_commandType = CommandType::Arithmetic;
    std::string   m_arg1;
    int16_t       m_arg2 = 0;
};
}  // namespace n2t

#endif
