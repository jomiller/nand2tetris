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

#include "Parser.h"

#include <Util.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <frozen/unordered_map.h>

#include <string_view>
#include <vector>

n2t::Parser::Parser(const std::filesystem::path& filename) : m_file{filename}
{
    throwIfNot<std::runtime_error>(m_file.good(), "Could not open input file ({})", filename.string());
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
        boost::algorithm::trim(currentCommand);
    }
    if (currentCommand.empty())
    {
        return false;
    }

    std::vector<std::string> args;
    boost::algorithm::split(args, currentCommand, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

    throwIfNot(!args.empty(), "Unable to parse command ({})", currentCommand);

    // clang-format off
    static constexpr auto commandTypes = frozen::make_unordered_map<frozen::string, CommandType>(
    {
        {"add",      CommandType::Arithmetic},
        {"sub",      CommandType::Arithmetic},
        {"neg",      CommandType::Arithmetic},
        {"and",      CommandType::Arithmetic},
        {"or",       CommandType::Arithmetic},
        {"not",      CommandType::Arithmetic},
        {"eq",       CommandType::Arithmetic},
        {"gt",       CommandType::Arithmetic},
        {"lt",       CommandType::Arithmetic},
        {"push",     CommandType::Push},
        {"pop",      CommandType::Pop},
        {"label",    CommandType::Label},
        {"goto",     CommandType::Goto},
        {"if-goto",  CommandType::If},
        {"function", CommandType::Function},
        {"return",   CommandType::Return},
        {"call",     CommandType::Call}
    });
    // clang-format on

    const auto iter = commandTypes.find(toFrozenString(args[0]));  // NOLINT(readability-qualified-auto)
    throwIfNot(iter != commandTypes.end(), "Invalid command type ({})", args[0]);

    m_commandType = iter->second;
    m_arg1.clear();
    m_arg2 = 0;
    if (m_commandType == CommandType::Arithmetic)
    {
        m_arg1.assign(iter->first.data(), iter->first.size());
    }
    else
    {
        if (args.size() > 1)
        {
            m_arg1 = args[1];
        }
        if (args.size() > 2)
        {
            try
            {
                m_arg2 = boost::lexical_cast<int16_t>(args[2]);
            }
            catch (const boost::bad_lexical_cast&)
            {
                throwAlways("Command argument ({}) is too large or is not an integer", args[2]);
            }

            throwIfNot(m_arg2 >= 0, "Command argument ({}) is a negative integer", args[2]);
        }
    }

    return true;
}

n2t::CommandType n2t::Parser::commandType() const
{
    return m_commandType;
}

const std::string& n2t::Parser::arg1() const
{
    return m_arg1;
}

int16_t n2t::Parser::arg2() const
{
    return m_arg2;
}
