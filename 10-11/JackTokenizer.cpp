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

#include "JackTokenizer.h"

#include "JackUtil.h"

#include <Assert.h>
#include <Util.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <frozen/unordered_set.h>

#include <algorithm>
#include <locale>
#include <string_view>
#include <utility>

n2t::JackTokenizer::TokenizerFunction::TokenizerFunction(unsigned int* lineNumber) : m_lineNumber{lineNumber}
{
}

void n2t::JackTokenizer::TokenizerFunction::reset()
{
    if (m_lineNumber)
    {
        *m_lineNumber = 0;
    }
    m_lineCount = 0;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4706)  // assignment within conditional expression
#endif

bool n2t::JackTokenizer::TokenizerFunction::operator()(std::istreambuf_iterator<char>& next,
                                                       std::istreambuf_iterator<char>  end,
                                                       std::string&                    tok)
{
    tok.clear();
    if (m_lineNumber)
    {
        *m_lineNumber += m_lineCount;
    }
    m_lineCount = 0;

    // skip past all whitespace and comments
    bool        isSpace = false;
    std::locale loc;
    while ((next != end) && ((isSpace = std::isspace(*next, loc)) || (*next == '/')))
    {
        if (isSpace)
        {
            if (*next == '\n')
            {
                ++m_lineCount;
            }
            ++next;
        }
        else  // (*next == '/')
        {
            char curr = *next;
            ++next;
            if (next == end)
            {
                tok += curr;
                return true;
            }

            if (*next == '/')  // comment to end of line
            {
                for (++next; (next != end) && (*next != '\n'); ++next)
                {
                }

                if (next != end)
                {
                    ++m_lineCount;
                    ++next;
                }
            }
            else if (*next == '*') /* comment until closing */ /** or API documentation comment */
            {
                curr = *next;
                ++next;
                if (next != end)
                {
                    curr = *next;
                }
                for (++next; (next != end) && ((curr != '*') || (*next != '/')); ++next)
                {
                    if (curr == '\n')
                    {
                        ++m_lineCount;
                    }
                    curr = *next;
                }
                if (next != end)
                {
                    ++next;
                }
            }
            else
            {
                tok += curr;
                return true;
            }
        }
    }

    if (next == end)
    {
        return false;
    }

    if (isDelimiter(*next))
    {
        if (*next == '"')  // string constant
        {
            do
            {
                tok += *next;
                ++next;
            }
            while ((next != end) && (*next != '\r') && (*next != '\n') && (*next != '"'));

            if (*next == '"')
            {
                tok += *next;
                ++next;
            }
        }
        else  // delimiter character
        {
            tok += *next;
            ++next;
        }
    }
    else
    {
        // append all the non-delimiter characters
        for (; (next != end) && !std::isspace(*next, loc) && !isDelimiter(*next); ++next)
        {
            tok += *next;
        }
    }
    return true;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

bool n2t::JackTokenizer::TokenizerFunction::isDelimiter(char c)
{
    static const std::string_view delimiters{"{}()[].,;+-*/&|~<=>\""};

    return (delimiters.find(c) != std::string_view::npos);
}

n2t::JackTokenizer::JackTokenizer(const std::filesystem::path& filename) :
    m_filename{filename.filename().string()},
    m_file{filename.string().data()}
{
    throwCond<std::runtime_error>(m_file.good(), "Could not open input file ({})", filename.string());

    m_fileIter = boost::make_token_iterator<std::string>(
        std::istreambuf_iterator<char>(m_file), std::istreambuf_iterator<char>(), TokenizerFunction(&m_lineNumber));

    m_lineNumber = 1;
}

bool n2t::JackTokenizer::hasMoreTokens() const
{
    static const TokenIterator fileEnd;
    return (m_fileIter != fileEnd);
}

void n2t::JackTokenizer::advance()
{
    auto currentToken = readNextToken();

    const auto key = toKeyword(currentToken);
    if (key.second)
    {
        m_tokenType = TokenType::Keyword;
        m_keyword   = key.first;
        return;
    }

    // clang-format off
    static constexpr auto symbols = frozen::make_unordered_set<char>(
    {
        '{', '}', '(', ')', '[', ']',
        '.', ',', ';', '+', '-', '*', '/',
        '&', '|', '~', '<', '=', '>'
    });
    // clang-format on

    if (currentToken.length() == 1)
    {
        const auto iter = symbols.find(currentToken.front());  // NOLINT(readability-qualified-auto)
        if (iter != symbols.end())
        {
            m_tokenType = TokenType::Symbol;
            m_symbol    = *iter;
            return;
        }
    }

    if (currentToken.front() == '"')
    {
        throwCond(
            currentToken.back() == '"', "Expected closing double quotation mark in string constant ({})", currentToken);

        m_tokenType = TokenType::StringConst;
        m_stringVal = currentToken.substr(/* __pos = */ 1, currentToken.length() - 2);
        return;
    }

    if (std::all_of(currentToken.begin(), currentToken.end(), boost::is_digit()))
    {
        try
        {
            m_intVal = boost::lexical_cast<int16_t>(currentToken);
        }
        catch (const boost::bad_lexical_cast&)
        {
            throwUncond("Integer constant ({}) is too large", currentToken);
        }
        m_tokenType = TokenType::IntConst;
    }
    else
    {
        throwCond(
            !std::isdigit(currentToken.front(), std::locale{}), "Identifier ({}) begins with a digit", currentToken);

        m_tokenType  = TokenType::Identifier;
        m_identifier = std::move(currentToken);
    }
}

n2t::TokenType n2t::JackTokenizer::tokenType() const
{
    return m_tokenType;
}

n2t::Keyword n2t::JackTokenizer::keyword() const
{
    N2T_ASSERT(m_tokenType == TokenType::Keyword);
    return m_keyword;
}

char n2t::JackTokenizer::symbol() const
{
    N2T_ASSERT(m_tokenType == TokenType::Symbol);
    return m_symbol;
}

const std::string& n2t::JackTokenizer::identifier() const
{
    N2T_ASSERT(m_tokenType == TokenType::Identifier);
    return m_identifier;
}

int16_t n2t::JackTokenizer::intVal() const
{
    N2T_ASSERT(m_tokenType == TokenType::IntConst);
    return m_intVal;
}

const std::string& n2t::JackTokenizer::stringVal() const
{
    N2T_ASSERT(m_tokenType == TokenType::StringConst);
    return m_stringVal;
}

std::string n2t::JackTokenizer::readNextToken()
{
    throwCond(hasMoreTokens(), "Unexpected end of file reached");
    return *m_fileIter++;
}
