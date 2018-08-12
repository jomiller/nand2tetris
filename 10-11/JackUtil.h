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

    template<typename Enum>
    static constexpr std::underlying_type_t<Enum> toUnderType(Enum enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(enumerator);
    }

    template<typename Exception = std::logic_error>
    [[noreturn]] static void throwUncond(std::string_view message,
                                         std::string_view filename   = {},
                                         unsigned int     lineNumber = 0)
    {
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

#define JACK_THROW_COND(condition, message) \
    if (!(condition))                       \
    {                                       \
        JackUtil::throwUncond(message);     \
    }                                       \
    else                                    \
        (void)0

#endif
