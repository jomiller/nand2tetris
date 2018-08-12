#include "XmlWriter.h"

#include "JackUtil.h"

#include <string_view>
#include <utility>

n2t::XmlWriter::Element::Element(XmlWriter* writer, const std::string& name) : m_writer(writer)
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
    m_filename(std::move(filename)),
    m_file(m_filename.string().data())
{
    JackUtil::throwCond<std::runtime_error>(m_file.good(), "Could not open output file (" + m_filename.string() + ")");

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
    m_file << m_indent << "<keyword> " << JackUtil::toString(keyword) << " </keyword>\n";
}

void n2t::XmlWriter::writeSymbol(char symbol)
{
    std::string_view xmlSymbol(&symbol, 1);

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

void n2t::XmlWriter::beginElement(const std::string& name)
{
    m_elementNames.push(name);
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
