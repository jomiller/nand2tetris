/*
 * This file is part of Nand2Tetris.
 *
 * Copyright (c) 2013-2018 Jonathan Miller
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

#include "VmWriter.h"

#include "JackUtil.h"

#include <cassert>
#include <map>
#include <string_view>
#include <utility>

n2t::VmWriter::VmWriter(std::filesystem::path filename) :
    m_filename(std::move(filename)),
    m_file(m_filename.string().data())
{
    JackUtil::throwCond<std::runtime_error>(m_file.good(), "Could not open output file (" + m_filename.string() + ")");
}

n2t::VmWriter::~VmWriter() noexcept
{
    try
    {
        if (!m_closed)
        {
            m_file.close();
            std::filesystem::remove(m_filename);
        }
    }
    catch (...)
    {
    }
}

void n2t::VmWriter::writePush(SegmentType segment, int16_t index)
{
    m_file << "push " << toString(segment) << " " << index << '\n';
}

void n2t::VmWriter::writePop(SegmentType segment, int16_t index)
{
    m_file << "pop " << toString(segment) << " " << index << '\n';
}

void n2t::VmWriter::writeArithmetic(ArithmeticCommand command)
{
    // clang-format off
    static const std::map<ArithmeticCommand, std::string_view> commands =
    {
        {ArithmeticCommand::Add, "add"},
        {ArithmeticCommand::Sub, "sub"},
        {ArithmeticCommand::Neg, "neg"},
        {ArithmeticCommand::And, "and"},
        {ArithmeticCommand::Or,  "or"},
        {ArithmeticCommand::Not, "not"},
        {ArithmeticCommand::Eq,  "eq"},
        {ArithmeticCommand::Gt,  "gt"},
        {ArithmeticCommand::Lt,  "lt"}
    };
    // clang-format on

    const auto cmd = commands.find(command);
    assert((cmd != commands.end()) && "Invalid arithmetic command");
    m_file << cmd->second << '\n';
}

void n2t::VmWriter::writeLabel(const std::string& label)
{
    m_file << "label " << label << '\n';
}

void n2t::VmWriter::writeGoto(const std::string& label)
{
    m_file << "goto " << label << '\n';
}

void n2t::VmWriter::writeIf(const std::string& label)
{
    m_file << "if-goto " << label << '\n';
}

void n2t::VmWriter::writeFunction(const std::string& functionName, unsigned int numLocals)
{
    m_file << "function " << functionName << " " << numLocals << '\n';
}

void n2t::VmWriter::writeReturn()
{
    m_file << "return\n";
}

void n2t::VmWriter::writeCall(const std::string& functionName, unsigned int numArguments)
{
    m_file << "call " << functionName << " " << numArguments << '\n';
}

void n2t::VmWriter::close()
{
    m_file.close();
    m_closed = true;
}

const std::string& n2t::VmWriter::toString(SegmentType segment)
{
    // clang-format off
    static const std::map<SegmentType, std::string> segments =
    {
        {SegmentType::Constant, "constant"},
        {SegmentType::Static,   "static"},
        {SegmentType::Pointer,  "pointer"},
        {SegmentType::Temp,     "temp"},
        {SegmentType::Argument, "argument"},
        {SegmentType::Local,    "local"},
        {SegmentType::This,     "this"},
        {SegmentType::That,     "that"}
    };
    // clang-format on

    const auto seg = segments.find(segment);
    assert((seg != segments.end()) && "Invalid memory segment");
    return seg->second;
}
