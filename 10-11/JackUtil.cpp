#include "JackUtil.h"

#include <cassert>
#include <map>

std::pair<n2t::Keyword, bool> n2t::JackUtil::toKeyword(const std::string& keyword)
{
    // clang-format off
    static const std::map<std::string_view, Keyword> keywords =
    {
        {"class",       Keyword::Class},
        {"constructor", Keyword::Constructor},
        {"function",    Keyword::Function},
        {"method",      Keyword::Method},
        {"int",         Keyword::Int},
        {"char",        Keyword::Char},
        {"boolean",     Keyword::Boolean},
        {"void",        Keyword::Void},
        {"static",      Keyword::Static},
        {"field",       Keyword::Field},
        {"var",         Keyword::Var},
        {"let",         Keyword::Let},
        {"do",          Keyword::Do},
        {"if",          Keyword::If},
        {"else",        Keyword::Else},
        {"while",       Keyword::While},
        {"return",      Keyword::Return},
        {"true",        Keyword::True},
        {"false",       Keyword::False},
        {"null",        Keyword::Null},
        {"this",        Keyword::This}
    };
    // clang-format on

    const auto key = keywords.find(keyword);
    if (key == keywords.end())
    {
        return std::make_pair(Keyword{0}, false);
    }
    return std::make_pair(key->second, true);
}

const std::string& n2t::JackUtil::toString(Keyword keyword)
{
    // clang-format off
    static const std::map<Keyword, std::string> keywords =
    {
        {Keyword::Class,       "class"},
        {Keyword::Constructor, "constructor"},
        {Keyword::Function,    "function"},
        {Keyword::Method,      "method"},
        {Keyword::Int,         "int"},
        {Keyword::Char,        "char"},
        {Keyword::Boolean,     "boolean"},
        {Keyword::Void,        "void"},
        {Keyword::Static,      "static"},
        {Keyword::Field,       "field"},
        {Keyword::Var,         "var"},
        {Keyword::Let,         "let"},
        {Keyword::Do,          "do"},
        {Keyword::If,          "if"},
        {Keyword::Else,        "else"},
        {Keyword::While,       "while"},
        {Keyword::Return,      "return"},
        {Keyword::True,        "true"},
        {Keyword::False,       "false"},
        {Keyword::Null,        "null"},
        {Keyword::This,        "this"}
    };
    // clang-format on

    const auto key = keywords.find(keyword);
    assert((key != keywords.end()) && "Invalid keyword");
    return key->second;
}

const std::string& n2t::JackUtil::toString(TokenType tokenType)
{
    // clang-format off
    static const std::map<TokenType, std::string> tokenTypes =
    {
        {TokenType::Keyword,     "keyword"},
        {TokenType::Symbol,      "symbol"},
        {TokenType::Identifier,  "identifier"},
        {TokenType::IntConst,    "integer constant"},
        {TokenType::StringConst, "string constant"}
    };
    // clang-format on

    const auto key = tokenTypes.find(tokenType);
    assert((key != tokenTypes.end()) && "Invalid token type");
    return key->second;
}
