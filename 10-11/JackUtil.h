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

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace n2t
{
struct SourceLocation
{
    std::string_view filename;
    unsigned int     lineNumber = 0;
};

class JackUtil
{
public:
    JackUtil() = delete;

    [[nodiscard]] static std::pair<Keyword, bool> toKeyword(std::string_view keyword);
    [[nodiscard]] static std::string_view         toString(Keyword keyword);
    [[nodiscard]] static std::string_view         toString(TokenType tokenType);
    [[nodiscard]] static std::string_view         toString(VariableKind kind);

    template<typename Enum>
    static constexpr std::underlying_type_t<Enum> toUnderlyingType(Enum enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(enumerator);
    }

    template<typename Exception = std::logic_error, typename... Args>
    [[noreturn]] static void throwUncond(SourceLocation sourceLocation, std::string_view message, Args&&... args)
    {
        static_assert(std::is_base_of_v<std::exception, Exception>);
        std::string msg;
        if (!sourceLocation.filename.empty())
        {
            msg = sourceLocation.filename;
            if (sourceLocation.lineNumber != 0)
            {
                msg.append(fmt::format(":{}", sourceLocation.lineNumber));
            }
            msg.append(": ");
        }
        msg.append(fmt::format(message, std::forward<Args>(args)...));
        throw Exception{msg};
    }

    template<typename Exception = std::logic_error, typename... Args>
    [[noreturn]] static void throwUncond(std::string_view message, Args&&... args)
    {
        throwUncond<Exception, Args...>(SourceLocation{}, message, std::forward<Args>(args)...);
    }

    template<typename Exception = std::logic_error, typename... Args>
    static void throwCond(bool condition, SourceLocation sourceLocation, std::string_view message, Args&&... args)
    {
        if (!condition)
        {
            throwUncond<Exception, Args...>(sourceLocation, message, std::forward<Args>(args)...);
        }
    }

    template<typename Exception = std::logic_error, typename... Args>
    static void throwCond(bool condition, std::string_view message, Args&&... args)
    {
        throwCond<Exception, Args...>(condition, SourceLocation{}, message, std::forward<Args>(args)...);
    }
};
}  // namespace n2t

#endif
