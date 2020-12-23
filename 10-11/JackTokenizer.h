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

#ifndef N2T_JACK_TOKENIZER_H
#define N2T_JACK_TOKENIZER_H

#include "JackTypes.h"

#include <boost/tokenizer.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace n2t
{
class JackTokenizer
{
public:
    class TokenizerFunction
    {
    public:
        TokenizerFunction() = default;
        explicit TokenizerFunction(unsigned int* lineNumber);

        void reset();

        [[nodiscard]] bool operator()(std::istreambuf_iterator<char>& next,
                                      std::istreambuf_iterator<char>  end,
                                      std::string&                    tok);

    private:
        [[nodiscard]] static bool isDelimiter(char c);

        unsigned int* m_lineNumber = nullptr;
        unsigned int  m_lineCount  = 0;
    };

    // Opens the input file and gets ready to tokenize it.
    explicit JackTokenizer(const std::filesystem::path& filename);

    // Returns the name of the input file.
    [[nodiscard]] const std::string& filename() const
    {
        return m_filename;
    }

    // Returns the current line number.
    [[nodiscard]] unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    // Are there more tokens in the input?
    [[nodiscard]] bool hasMoreTokens() const;

    // Gets the next token from the input and makes it the current token.
    // Should be called only if there are more tokens in the input.
    void advance();

    // Returns the type of the current token.
    [[nodiscard]] TokenType tokenType() const;

    // Returns the keyword which is the current token.
    // Should be called only if the current token type is Keyword.
    [[nodiscard]] Keyword keyword() const;

    // Returns the symbol which is the current token.
    // Should be called only if the current token type is Symbol.
    [[nodiscard]] char symbol() const;

    // Returns the identifier which is the current token.
    // Should be called only if the current token type is Identifier.
    [[nodiscard]] const std::string& identifier() const;

    // Returns the integer value of the current token.
    // Should be called only if the current token type is IntConst.
    [[nodiscard]] int16_t intVal() const;

    // Returns the string value of the current token, without the double quotes.
    // Should be called only if the current token type is StringConst.
    [[nodiscard]] const std::string& stringVal() const;

private:
    using TokenIterator = boost::token_iterator_generator<TokenizerFunction, std::istreambuf_iterator<char>>::type;

    std::string readNextToken();

    std::string   m_filename;
    std::ifstream m_file;
    TokenIterator m_fileIter;
    unsigned int  m_lineNumber = 0;
    TokenType     m_tokenType  = TokenType::Keyword;
    Keyword       m_keyword    = Keyword::Class;
    char          m_symbol     = 0;
    std::string   m_identifier;
    int16_t       m_intVal = 0;
    std::string   m_stringVal;
};
}  // namespace n2t

#endif
