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

#ifndef VM_UTIL_H
#define VM_UTIL_H

#include "VmTypes.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace n2t
{
class VmUtil
{
public:
    VmUtil() = delete;

    static const std::string& toString(CommandType command);

    template<typename Exception = std::logic_error>
    [[noreturn]] static void throwUncond(std::string_view message,
                                         std::string_view filename   = {},
                                         unsigned int     lineNumber = 0)
    {
        static_assert(std::is_base_of_v<std::exception, Exception>);
        std::string msg;
        if (!filename.empty())
        {
            msg = filename;
            if (lineNumber != 0)
            {
                msg += ":" + std::to_string(lineNumber);
            }
            msg += ": ";
        }
        msg += message;
        throw Exception{msg};
    }

    template<typename Exception = std::logic_error>
    static void throwCond(bool             condition,
                          std::string_view message,
                          std::string_view filename   = {},
                          unsigned int     lineNumber = 0)
    {
        if (!condition)
        {
            throwUncond<Exception>(message, filename, lineNumber);
        }
    }
};
}  // namespace n2t

#define N2T_VM_THROW_COND(condition, message) \
    if (!(condition))                         \
    {                                         \
        VmUtil::throwUncond(message);         \
    }                                         \
    else                                      \
        (void)0

#endif
