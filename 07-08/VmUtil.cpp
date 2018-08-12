#include "VmUtil.h"

#include <cassert>
#include <map>

const std::string& n2t::VmUtil::toString(CommandType command)
{
    // clang-format off
    static const std::map<CommandType, std::string> commands =
    {
        {CommandType::Arithmetic, "arithmetic"},
        {CommandType::Push,       "push"},
        {CommandType::Pop,        "pop"},
        {CommandType::Label,      "label"},
        {CommandType::Goto,       "goto"},
        {CommandType::If,         "if-goto"},
        {CommandType::Function,   "function"},
        {CommandType::Return,     "return"},
        {CommandType::Call,       "call"}
    };
    // clang-format on

    const auto key = commands.find(command);
    assert((key != commands.end()) && "Invalid command type");
    return key->second;
}
