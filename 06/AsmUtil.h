#ifndef ASM_UTIL_H
#define ASM_UTIL_H

#include <stdexcept>
#include <string>
#include <string_view>

namespace n2t
{
class AsmUtil
{
public:
    AsmUtil() = delete;

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

#define ASM_THROW_COND(condition, message) \
    if (!(condition))                      \
    {                                      \
        AsmUtil::throwUncond(message);     \
    }                                      \
    else                                   \
        (void)0

#endif
