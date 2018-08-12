#include "Parser.h"

#include "AsmUtil.h"

#include <boost/algorithm/string/classification.hpp>

#include <algorithm>

n2t::Parser::Parser(const std::string& filename) : m_file(filename.data())
{
    AsmUtil::throwCond<std::runtime_error>(m_file.good(), "Could not open input file (" + filename + ")");
}

bool n2t::Parser::advance()
{
    std::string currentCommand;
    while (currentCommand.empty() && std::getline(m_file, currentCommand))
    {
        ++m_lineNumber;

        const auto commentPos = currentCommand.find("//");
        if (commentPos != std::string::npos)
        {
            currentCommand.erase(commentPos);
        }
        currentCommand.erase(std::remove_if(currentCommand.begin(), currentCommand.end(), boost::algorithm::is_space()),
                             currentCommand.end());
    }
    if (currentCommand.empty())
    {
        return false;
    }

    if (currentCommand.front() == '@')
    {
        ASM_THROW_COND(currentCommand.length() != 1, "Address or symbol not specified in addressing instruction");

        m_commandType = CommandType::A;
        m_symbol      = currentCommand.substr(1);  // @Xxx
    }
    else if (currentCommand.front() == '(')
    {
        ASM_THROW_COND(currentCommand.back() == ')',
                       "Expected closing parenthesis in label command (" + currentCommand + ")");

        ASM_THROW_COND(currentCommand.length() != 2, "Symbol not specified in label command");

        m_commandType = CommandType::L;
        m_symbol      = currentCommand.substr(1, currentCommand.length() - 2);  // (Xxx)
    }
    else
    {
        m_commandType = CommandType::C;

        auto equalsignPos = currentCommand.find('=');
        auto semicolonPos = currentCommand.find(';');

        m_dest.clear();
        if (equalsignPos != std::string::npos)
        {
            m_dest = currentCommand.substr(0, equalsignPos);
        }

        m_jump.clear();
        if (semicolonPos != std::string::npos)
        {
            m_jump = currentCommand.substr(semicolonPos + 1);
        }

        if (equalsignPos == std::string::npos)
        {
            equalsignPos = 0;
        }
        else
        {
            ++equalsignPos;
        }
        if (semicolonPos != std::string::npos)
        {
            semicolonPos -= equalsignPos;
        }
        m_comp = currentCommand.substr(equalsignPos, semicolonPos);
    }

    return true;
}

n2t::CommandType n2t::Parser::commandType() const
{
    return m_commandType;
}

const std::string& n2t::Parser::symbol() const
{
    return m_symbol;
}

const std::string& n2t::Parser::dest() const
{
    return m_dest;
}

const std::string& n2t::Parser::comp() const
{
    return m_comp;
}

const std::string& n2t::Parser::jump() const
{
    return m_jump;
}
