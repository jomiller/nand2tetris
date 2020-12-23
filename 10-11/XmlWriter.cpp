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

#include "XmlWriter.h"

#include "JackUtil.h"

#include <Util.h>

#include <utility>

n2t::XmlWriter::Element::Element(XmlWriter* writer, std::string_view name) : m_writer{writer}
{
    if (m_writer)
    {
        m_writer->beginElement(name);
    }
}

n2t::XmlWriter::Element::~Element() noexcept
{
    try
    {
        if (m_writer)
        {
            m_writer->endElement();
        }
    }
    catch (...)
    {
    }
}

n2t::XmlWriter::XmlWriter(std::filesystem::path filename) :
    m_filename{std::move(filename)},
    m_file{m_filename.string().data()}
{
    throwIfNot<std::runtime_error>(m_file.good(), "Could not open output file ({})", m_filename.string());

    m_file << R"(<?xml version="1.0" encoding="UTF-8"?>)" << '\n';
}

n2t::XmlWriter::~XmlWriter() noexcept
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

void n2t::XmlWriter::writeKeyword(Keyword keyword)
{
    m_file << m_indent << "<keyword> " << toString(keyword) << " </keyword>\n";
}

void n2t::XmlWriter::writeSymbol(char symbol)
{
    std::string_view xmlSymbol{&symbol, /* __len = */ 1};

    // clang-format off
    switch (symbol)
    {
        case '<': xmlSymbol = "&lt;"; break;
        case '>': xmlSymbol = "&gt;"; break;
        case '&': xmlSymbol = "&amp;"; break;
        default: break;
    }
    // clang-format on

    m_file << m_indent << "<symbol> " << xmlSymbol << " </symbol>\n";
}

void n2t::XmlWriter::writeIdentifier(const std::string& identifier)
{
    m_file << m_indent << "<identifier> " << identifier << " </identifier>\n";
}

void n2t::XmlWriter::writeIntegerConstant(int16_t intConst)
{
    m_file << m_indent << "<integerConstant> " << intConst << " </integerConstant>\n";
}

void n2t::XmlWriter::writeStringConstant(const std::string& stringConst)
{
    m_file << m_indent << "<stringConstant> " << stringConst << " </stringConstant>\n";
}

void n2t::XmlWriter::beginElement(std::string_view name)
{
    m_elementNames.emplace(name);
    m_file << m_indent << "<" << m_elementNames.top() << ">\n";
    m_indent += "  ";
}

void n2t::XmlWriter::endElement()
{
    m_indent.erase(m_indent.length() - 2);
    m_file << m_indent << "</" << m_elementNames.top() << ">\n";
    m_elementNames.pop();
}

void n2t::XmlWriter::close()
{
    m_file.close();
    m_closed = true;
}
