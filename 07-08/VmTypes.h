#ifndef VM_TYPES_H
#define VM_TYPES_H

#include <filesystem>
#include <vector>

namespace n2t
{
using PathList = std::vector<std::filesystem::path>;

enum class CommandType
{
    Arithmetic,
    Push,
    Pop,
    Label,
    Goto,
    If,
    Function,
    Return,
    Call
};

enum class SegmentType
{
    Constant,
    Static,
    Pointer,
    Temp,
    Argument,
    Local,
    This,
    That
};
}  // namespace n2t

#endif
