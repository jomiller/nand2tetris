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

#ifndef JACK_UTIL_H
#define JACK_UTIL_H

#include "JackTypes.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace n2t
{
class JackUtil
{
public:
    JackUtil() = delete;

    static std::pair<Keyword, bool> toKeyword(const std::string& keyword);
    static const std::string&       toString(Keyword keyword);
    static const std::string&       toString(TokenType tokenType);
    static const std::string&       toString(VariableKind kind);

    template<typename Enum>
    static constexpr std::underlying_type_t<Enum> toUnderlyingType(Enum enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(enumerator);
    }

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
        throw Exception(msg);
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

#define N2T_JACK_THROW_COND(condition, message) \
    if (!(condition))                           \
    {                                           \
        JackUtil::throwUncond(message);         \
    }                                           \
    else                                        \
        (void)0

#endif
