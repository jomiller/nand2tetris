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

#include "JackUtil.h"

#include <Assert.h>
#include <Util.h>

#include <frozen/unordered_map.h>

std::pair<n2t::Keyword, bool> n2t::toKeyword(std::string_view keyword)
{
    // clang-format off
    static constexpr auto keywords = frozen::make_unordered_map<frozen::string, Keyword>(
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
    });
    // clang-format on

    const auto iter = keywords.find(toFrozenString(keyword));  // NOLINT(readability-qualified-auto)
    if (iter == keywords.end())
    {
        return std::make_pair(Keyword{0}, false);
    }
    return std::make_pair(iter->second, true);
}

std::string_view n2t::toString(Keyword keyword)
{
    // clang-format off
    static constexpr auto keywords = frozen::make_unordered_map<Keyword, std::string_view>(
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
    });
    // clang-format on

    const auto iter = keywords.find(keyword);  // NOLINT(readability-qualified-auto)
    N2T_ASSERT((iter != keywords.end()) && "Invalid keyword");
    return iter->second;
}

std::string_view n2t::toString(TokenType tokenType)
{
    // clang-format off
    static constexpr auto tokenTypes = frozen::make_unordered_map<TokenType, std::string_view>(
    {
        {TokenType::Keyword,     "keyword"},
        {TokenType::Symbol,      "symbol"},
        {TokenType::Identifier,  "identifier"},
        {TokenType::IntConst,    "integer constant"},
        {TokenType::StringConst, "string constant"}
    });
    // clang-format on

    const auto iter = tokenTypes.find(tokenType);  // NOLINT(readability-qualified-auto)
    N2T_ASSERT((iter != tokenTypes.end()) && "Invalid token type");
    return iter->second;
}

std::string_view n2t::toString(VariableKind kind)
{
    // clang-format off
    static constexpr auto variableKinds = frozen::make_unordered_map<VariableKind, std::string_view>(
    {
        {VariableKind::Static,   "static"},
        {VariableKind::Field,    "field"},
        {VariableKind::Argument, "argument"},
        {VariableKind::Local,    "local"}
    });
    // clang-format on

    const auto iter = variableKinds.find(kind);  // NOLINT(readability-qualified-auto)
    N2T_ASSERT((iter != variableKinds.end()) && "Invalid variable kind");
    return iter->second;
}
