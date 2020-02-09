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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    assert((key != tokenTypes.end()) && "Invalid token type");
    return key->second;
}

const std::string& n2t::JackUtil::toString(VariableKind kind)
{
    // clang-format off
    static const std::map<VariableKind, std::string> variableKinds =
    {
        {VariableKind::Static,   "static"},
        {VariableKind::Field,    "field"},
        {VariableKind::Argument, "argument"},
        {VariableKind::Local,    "local"}
    };
    // clang-format on

    const auto key = variableKinds.find(kind);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    assert((key != variableKinds.end()) && "Invalid variable kind");
    return key->second;
}
