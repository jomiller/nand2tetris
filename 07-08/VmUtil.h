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

#define VM_THROW_COND(condition, message) \
    if (!(condition))                     \
    {                                     \
        VmUtil::throwUncond(message);     \
    }                                     \
    else                                  \
        (void)0

#endif
