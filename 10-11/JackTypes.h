#ifndef JACK_TYPES_H
#define JACK_TYPES_H

#include <filesystem>
#include <vector>

namespace n2t
{
using PathList = std::vector<std::filesystem::path>;

enum class TokenType
{
    Keyword,
    Symbol,
    Identifier,
    IntConst,
    StringConst
};

enum class Keyword
{
    Class,
    Constructor,
    Function,
    Method,
    Int,
    Char,
    Boolean,
    Void,
    Static,
    Field,
    Var,
    Let,
    Do,
    If,
    Else,
    While,
    Return,
    True,
    False,
    Null,
    This
};

enum class VariableKind
{
    Static,
    Field,
    Argument,
    Local,
    None,
    Count = None
};
}  // namespace n2t

#endif
