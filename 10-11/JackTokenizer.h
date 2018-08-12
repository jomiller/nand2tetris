#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

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
        explicit TokenizerFunction(unsigned int* lineNumber = nullptr);

        void reset();
        bool operator()(std::istreambuf_iterator<char>& next, std::istreambuf_iterator<char> end, std::string& tok);

    private:
        static bool isDelimiter(char c);

        unsigned int* m_lineNumber;
        unsigned int  m_lineCount = 0;
    };

    // Opens the input file and gets ready to tokenize it.
    explicit JackTokenizer(const std::filesystem::path& filename);

    // Returns the name of the input file.
    const std::string& filename() const
    {
        return m_filename;
    }

    // Returns the current line number.
    unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    // Are there more tokens in the input?
    bool hasMoreTokens() const;

    // Gets the next token from the input and makes it the current token.
    // Should be called only if there are more tokens in the input.
    void advance();

    // Returns the type of the current token.
    TokenType tokenType() const;

    // Returns the keyword which is the current token.
    // Should be called only if the current token type is Keyword.
    Keyword keyword() const;

    // Returns the symbol which is the current token.
    // Should be called only if the current token type is Symbol.
    char symbol() const;

    // Returns the identifier which is the current token.
    // Should be called only if the current token type is Identifier.
    const std::string& identifier() const;

    // Returns the integer value of the current token.
    // Should be called only if the current token type is IntConst.
    int16_t intVal() const;

    // Returns the string value of the current token, without the double quotes.
    // Should be called only if the current token type is StringConst.
    const std::string& stringVal() const;

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
