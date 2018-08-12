#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <fstream>
#include <string>

namespace n2t
{
enum class CommandType
{
    A,  // @Xxx
    C,  // dest=comp;jump
    L   // (Xxx)
};

class Parser
{
public:
    // Opens the input file and gets ready to parse it.
    explicit Parser(const std::string& filename);

    // Returns the current line number.
    unsigned int lineNumber() const
    {
        return m_lineNumber;
    }

    // Reads the next command from the input and makes it the current command.
    bool advance();

    // Returns the type of the current command.
    CommandType commandType() const;

    // Returns the symbol or decimal Xxx of the current command @Xxx or (Xxx).
    // Should be called only when commandType() is CommandType::A or CommandType::L.
    const std::string& symbol() const;

    // Returns the 'dest' mnemonic in the current C-command (8 possibilities).
    // Should be called only when commandType() is CommandType::C.
    const std::string& dest() const;

    // Returns the 'comp' mnemonic in the current C-command (28 possibilities).
    // Should be called only when commandType() is CommandType::C.
    const std::string& comp() const;

    // Returns the 'jump' mnemonic in the current C-command (8 possibilities).
    // Should be called only when commandType() is CommandType::C.
    const std::string& jump() const;

private:
    std::ifstream m_file;
    unsigned int  m_lineNumber  = 0;
    CommandType   m_commandType = CommandType::A;
    std::string   m_symbol;
    std::string   m_dest;
    std::string   m_comp;
    std::string   m_jump;
};
}  // namespace n2t

#endif
