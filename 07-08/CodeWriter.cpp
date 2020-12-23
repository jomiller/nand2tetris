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

#include "CodeWriter.h"

#include <Assert.h>
#include <Util.h>

#include <fmt/format.h>

#include <frozen/unordered_map.h>

#include <array>
#include <locale>

n2t::CodeWriter::CodeWriter(std::filesystem::path filename) :
    m_outputFilename{std::move(filename)},
    m_file{m_outputFilename.string().data()}
{
    throwIfNot<std::runtime_error>(m_file.good(), "Could not open output file ({})", m_outputFilename.string());
}

n2t::CodeWriter::~CodeWriter() noexcept
{
    try
    {
        if (!m_closed)
        {
            m_file.close();
            std::filesystem::remove(m_outputFilename);
        }
    }
    catch (...)
    {
    }
}

void n2t::CodeWriter::setFilename(std::string_view inputFilename)
{
    validateFunction();
    m_currentInputFilename = inputFilename.substr(/* __pos = */ 0, inputFilename.rfind('.') + 1);
}

void n2t::CodeWriter::writeInit()
{
    // initialize the stack pointer to 0x0100
    // clang-format off
    m_file << "@256\n"
           << "D=A\n"
           << "@SP\n"
           << "M=D\n";
    // clang-format on

    // start executing the translated code of Sys.init()
    writeCall("Sys.init", /* numArguments = */ 0);
}

void n2t::CodeWriter::writeArithmetic(const std::string& command)
{
    struct ArithmeticInfo
    {
        constexpr ArithmeticInfo(bool u, bool l, std::string_view i) : unary{u}, logic{l}, inst{i}
        {
        }

        bool             unary;
        bool             logic;
        std::string_view inst;
    };

    // clang-format off
    static constexpr auto arithmeticInfo = frozen::make_unordered_map<frozen::string, ArithmeticInfo>(
    {
        {"add", ArithmeticInfo{/* u = */ false, /* l = */ false, "M=D+M"}},
        {"sub", ArithmeticInfo{/* u = */ false, /* l = */ false, "M=M-D"}},
        {"neg", ArithmeticInfo{/* u = */ true,  /* l = */ false, "M=-M"}},
        {"and", ArithmeticInfo{/* u = */ false, /* l = */ false, "M=D&M"}},
        {"or",  ArithmeticInfo{/* u = */ false, /* l = */ false, "M=D|M"}},
        {"not", ArithmeticInfo{/* u = */ true,  /* l = */ false, "M=!M"}},
        {"lt",  ArithmeticInfo{/* u = */ false, /* l = */ true,  "D;JLT"}},
        {"eq",  ArithmeticInfo{/* u = */ false, /* l = */ true,  "D;JEQ"}},
        {"gt",  ArithmeticInfo{/* u = */ false, /* l = */ true,  "D;JGT"}}
    });
    // clang-format on

    const auto iter = arithmeticInfo.find(toFrozenString(command));  // NOLINT(readability-qualified-auto)
    throwIfNot(iter != arithmeticInfo.end(), "Invalid arithmetic command type ({})", command);

    const auto& info = iter->second;
    if (info.unary)
    {
        // clang-format off
        m_file << "@SP\n"
               << "A=M-1\n"
               << info.inst << '\n';
        // clang-format on
    }
    else
    {
        // clang-format off
        m_file << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "A=A-1\n";
        // clang-format on

        if (info.logic)
        {
            const auto label = makeNumberedLabel("LOGIC");

            // clang-format off
            m_file << "D=M-D\n"
                   << "M=-1\n"
                   << "@" << label << '\n'
                   << info.inst << '\n'
                   << "@SP\n"
                   << "A=M-1\n"
                   << "M=0\n"
                   << "(" << label << ")\n";
            // clang-format on
        }
        else
        {
            m_file << info.inst << '\n';
        }
    }
}

void n2t::CodeWriter::writePushPop(CommandType command, const std::string& segment, int16_t index)
{
    struct SegmentInfo
    {
        constexpr SegmentInfo(SegmentType t, bool i, std::string_view n = {}, int16_t a = 0) :
            type{t},
            indirect{i},
            name{n},
            address{a}
        {
        }

        SegmentType      type;
        bool             indirect;
        std::string_view name;
        int16_t          address;
    };

    // clang-format off
    static constexpr auto segmentInfo = frozen::make_unordered_map<frozen::string, SegmentInfo>(
    {
        {"constant", SegmentInfo{SegmentType::Constant, /* i = */ false}},
        {"static",   SegmentInfo{SegmentType::Static,   /* i = */ false}},
        {"pointer",  SegmentInfo{SegmentType::Pointer,  /* i = */ false,   "R",     /* a = */ 0x0003}},
        {"temp",     SegmentInfo{SegmentType::Temp,     /* i = */ false,   "R",     /* a = */ 0x0005}},
        {"argument", SegmentInfo{SegmentType::Argument, /* i = */ true,    "ARG"}},
        {"local",    SegmentInfo{SegmentType::Local,    /* i = */ true,    "LCL"}},
        {"this",     SegmentInfo{SegmentType::This,     /* i = */ true,    "THIS"}},
        {"that",     SegmentInfo{SegmentType::That,     /* i = */ true,    "THAT"}}
    });
    // clang-format on

    const auto iter = segmentInfo.find(toFrozenString(segment));  // NOLINT(readability-qualified-auto)
    throwIfNot(iter != segmentInfo.end(), "Invalid memory segment ({})", segment);

    const auto& info = iter->second;
    std::string symbol{info.name};
    switch (info.type)
    {
        case SegmentType::Constant:
            symbol = std::to_string(index);
            break;

        case SegmentType::Static:
            symbol = m_currentInputFilename + std::to_string(index);
            break;

        case SegmentType::Pointer:
        case SegmentType::Temp:
            symbol += std::to_string(info.address + index);
            break;

        case SegmentType::Argument:
            if (!m_currentFunction.name.empty())
            {
                m_currentFunction.numParameters = std::max<int16_t>(m_currentFunction.numParameters, index + 1);
            }
            break;

        default:
            break;
    }

    if (command == CommandType::Push)
    {
        // read the value from the source memory segment into D
        m_file << "@" << symbol << '\n';
        if (info.type == SegmentType::Constant)
        {
            m_file << "D=A\n";
        }
        else
        {
            if (info.indirect)
            {
                // compute the source address
                if (index > 1)
                {
                    // clang-format off
                    m_file << "D=M\n"
                           << "@" << index << '\n'
                           << "A=D+A\n";
                    // clang-format on
                }
                else
                {
                    m_file << "A=M" << ((index == 1) ? "+1" : "") << '\n';
                }
            }
            m_file << "D=M\n";
        }

        // push the value from D onto the stack
        pushFromD();
    }
    else  // (command == CommandType::Pop)
    {
        throwIfNot(info.type != SegmentType::Constant, "Cannot pop to the constant segment");

        if (info.indirect && (index > 1))
        {
            // pre-compute the destination address and store it in R13
            // clang-format off
            m_file << "@" << symbol << '\n'
                   << "D=M\n"
                   << "@" << index << '\n'
                   << "D=D+A\n"
                   << "@R13\n"
                   << "M=D\n";
            // clang-format on

            // change the location of the destination address to R13
            symbol = "R13";
        }

        // pop the value from the stack into D
        popToD();

        // write the value from D into the destination memory segment
        m_file << "@" << symbol << '\n';
        if (info.indirect)
        {
            // compute the destination address
            m_file << "A=M" << ((index == 1) ? "+1" : "") << '\n';
        }
        m_file << "M=D\n";
    }
}

void n2t::CodeWriter::writeLabel(const std::string& label)
{
    throwIfNot(!std::isdigit(label.front(), std::locale{}), "Label ({}) begins with a digit", label);

    if (!m_labels.insert(label).second)
    {
        auto msg = fmt::format("Label ({}) already exists", label);
        if (!m_currentFunction.name.empty())
        {
            msg.append(fmt::format(" in function ({})", m_currentFunction.name));
        }
        throwAlways(msg);
    }
    m_file << "(" << makeLabel(label) << ")\n";
}

void n2t::CodeWriter::writeGoto(const std::string& label)
{
    m_gotoDestinations.insert(label);

    // clang-format off
    m_file << "@" << makeLabel(label) << '\n'
           << "0;JMP\n";
    // clang-format on
}

void n2t::CodeWriter::writeIf(const std::string& label)
{
    m_gotoDestinations.insert(label);

    // pop the Boolean value from the stack into D
    popToD();

    // goto the label if the value in D is non-zero
    // clang-format off
    m_file << "@" << makeLabel(label) << '\n'
           << "D;JNE\n";
    // clang-format on
}

void n2t::CodeWriter::writeFunction(const std::string& functionName, int16_t numLocals)
{
    N2T_ASSERT((numLocals >= 0) && "Number of function local variables is negative");

    validateFunction();

    throwIfNot(
        !std::isdigit(functionName.front(), std::locale{}), "Function name ({}) begins with a digit", functionName);

    throwIfNot(m_definedFunctions.find(functionName) == m_definedFunctions.end(),
               "Function with name ({}) already exists",
               functionName);

    m_currentFunction.name          = functionName;
    m_currentFunction.numParameters = 0;

    // declare a label for the function entry point
    m_file << "(" << functionName << ")\n";

    // allocate and initialize the local variables
    if (numLocals > 0)
    {
        m_file << "D=0\n";
    }
    for (int16_t lcl = 0; lcl < numLocals; ++lcl)
    {
        pushFromD();
    }
}

void n2t::CodeWriter::writeReturn()
{
    throwIfNot(!m_currentFunction.name.empty(), "Return command is outside of a function");

    // save the base address of the calling function's saved state into R13
    // clang-format off
    m_file << "@LCL\n"
           << "D=M\n"
           << "@5\n"
           << "D=D-A\n"
           << "@R13\n"
           << "AM=D\n";
    // clang-format on

    // save the return address into R14 as it will be overwritten by the return value
    // if the called function has no arguments
    // clang-format off
    m_file << "D=M\n"
           << "@R14\n"
           << "M=D\n";
    // clang-format on

    // write the return value into the first element of the 'argument' memory segment,
    // overwriting the first argument (or the return address if the function has no arguments)
    popToD();
    // clang-format off
    m_file << "@ARG\n"
           << "A=M\n"
           << "M=D\n";
    // clang-format on

    // restore the stack pointer of the calling function
    // clang-format off
    m_file << "@ARG\n"
           << "D=M+1\n"
           << "@SP\n"
           << "M=D\n";
    // clang-format on

    // restore the 'local', 'argument', 'this' and 'that' memory segments of the calling function
    // clang-format off
    static const auto segments = std::array
    {
        "LCL",
        "ARG",
        "THIS",
        "THAT"
    };
    // clang-format on

    for (const auto* seg : segments)
    {
        // clang-format off
        m_file << "@R13\n"
               << "AM=M+1\n"
               << "D=M\n"
               << "@" << seg << '\n'
               << "M=D\n";
        // clang-format on
    }

    // goto the return address (in the calling function's code)
    // clang-format off
    m_file << "@R14\n"
           << "A=M\n"
           << "0;JMP\n";
    // clang-format on
}

void n2t::CodeWriter::writeCall(const std::string& functionName, int16_t numArguments)
{
    N2T_ASSERT((numArguments >= 0) && "Number of function arguments is negative");

    m_calledFunctions.emplace(functionName, numArguments);
    const auto label = makeNumberedLabel("RETURN");

    // push the return address onto the stack
    // clang-format off
    m_file << "@" << label << '\n'
           << "D=A\n";
    // clang-format on

    pushFromD();

    // save the 'local', 'argument', 'this' and 'that' memory segments of the calling function
    // clang-format off
    static const auto segments = std::array
    {
        "LCL",
        "ARG",
        "THIS",
        "THAT"
    };
    // clang-format on

    for (const auto* seg : segments)
    {
        // clang-format off
        m_file << "@" << seg << '\n'
               << "D=M\n";
        // clang-format on

        pushFromD();
    }

    const int16_t savedStateSize = 1 + static_cast<int16_t>(segments.size());

    // reposition the 'argument' memory segment
    // clang-format off
    m_file << "@SP\n"
           << "D=M\n"
           << "@" << (numArguments + savedStateSize) << '\n'
           << "D=D-A\n"
           << "@ARG\n"
           << "M=D\n";
    // clang-format on

    // reposition the 'local' memory segment
    // clang-format off
    m_file << "@SP\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n";
    // clang-format on

    // transfer control to the function
    // clang-format off
    m_file << "@" << functionName << '\n'
           << "0;JMP\n";
    // clang-format on

    // declare a label for the return address
    m_file << "(" << label << ")\n";
}

void n2t::CodeWriter::close()
{
    m_file.close();
    validateFunction();
    validateFunctionCalls();
    m_closed = true;
}

void n2t::CodeWriter::pushFromD()
{
    // clang-format off
    m_file << "@SP\n"
           << "AM=M+1\n"
           << "A=A-1\n"
           << "M=D\n";
    // clang-format on
}

void n2t::CodeWriter::popToD()
{
    // clang-format off
    m_file << "@SP\n"
           << "AM=M-1\n"
           << "D=M\n";
    // clang-format on
}

unsigned int n2t::CodeWriter::getNextLabelId()
{
    return m_nextLabelId++;
}

std::string n2t::CodeWriter::makeLabel(const std::string& label) const
{
    return fmt::format("{}${}", m_currentFunction.name, label);
}

std::string n2t::CodeWriter::makeNumberedLabel(std::string_view label)
{
    return fmt::format("{}${}{}", m_currentFunction.name, label, getNextLabelId());
}

void n2t::CodeWriter::validateFunction()
{
    // validate goto destinations
    for (const auto& label : m_gotoDestinations)
    {
        if (m_labels.find(label) == m_labels.end())
        {
            auto msg = fmt::format("Undefined reference to label ({})", label);
            if (!m_currentFunction.name.empty())
            {
                msg.append(fmt::format(" in function ({})", m_currentFunction.name));
            }
            throwAlways(msg);
        }
    }
    m_labels.clear();
    m_gotoDestinations.clear();
    m_nextLabelId = 0;

    if (!m_currentFunction.name.empty())
    {
        m_definedFunctions.emplace(m_currentFunction.name, m_currentFunction.numParameters);
        m_currentFunction.name.clear();
    }
}

void n2t::CodeWriter::validateFunctionCalls() const
{
    for (const FunctionCallInfo& call : m_calledFunctions)
    {
        const auto func = m_definedFunctions.find(call.name);
        throwIfNot(func != m_definedFunctions.end(), "Undefined reference to function ({})", call.name);

        throwIfNot(call.numArguments >= func->second,
                   "Function ({}) requires at least {} argument(s) but called with {} argument(s)",
                   func->first,
                   func->second,
                   call.numArguments);
    }
}
