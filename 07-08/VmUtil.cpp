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

#include "VmUtil.h"

#include <cassert>
#include <map>

const std::string& n2t::VmUtil::toString(CommandType command)
{
    // clang-format off
    static const std::map<CommandType, std::string> commands =
    {
        {CommandType::Arithmetic, "arithmetic"},
        {CommandType::Push,       "push"},
        {CommandType::Pop,        "pop"},
        {CommandType::Label,      "label"},
        {CommandType::Goto,       "goto"},
        {CommandType::If,         "if-goto"},
        {CommandType::Function,   "function"},
        {CommandType::Return,     "return"},
        {CommandType::Call,       "call"}
    };
    // clang-format on

    const auto key = commands.find(command);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    assert((key != commands.end()) && "Invalid command type");
    return key->second;
}
