#ifndef PARSER_H
#define PARSER_H

#include "VmTypes.h"

#include <cstdint>
#include <fstream>
#include <string>

namespace n2t
{
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

    // Returns the type of the current VM command.
    // Arithmetic is returned for all the arithmetic commands.
    CommandType commandType() const;

    // Returns the first argument of the current command.
    // In the case of Arithmetic, the command itself (add, sub, etc.) is returned.
    // Should not be called if the current command is Return
    const std::string& arg1() const;

    // Returns the second argument of the current command.
    // Should be called only if the current command is Push, Pop, Function, or Call.
    int16_t arg2() const;

private:
    std::ifstream m_file;
    unsigned int  m_lineNumber  = 0;
    CommandType   m_commandType = CommandType::Arithmetic;
    std::string   m_arg1;
    int16_t       m_arg2 = 0;
};
}  // namespace n2t

#endif
