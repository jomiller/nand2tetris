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

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include "JackTypes.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stack>
#include <string>

namespace n2t
{
class XmlWriter
{
public:
    class Element
    {
    public:
        Element(XmlWriter* writer, const std::string& name);

        Element(const Element&) = delete;
        Element(Element&&)      = delete;

        Element& operator=(const Element&) = delete;
        Element& operator=(Element&&) = delete;

        ~Element() noexcept;

    private:
        XmlWriter* m_writer = nullptr;
    };

    explicit XmlWriter(std::filesystem::path filename);

    XmlWriter(const XmlWriter&) = delete;
    XmlWriter(XmlWriter&&)      = delete;

    XmlWriter& operator=(const XmlWriter&) = delete;
    XmlWriter& operator=(XmlWriter&&) = delete;

    ~XmlWriter() noexcept;

    void writeKeyword(Keyword keyword);
    void writeSymbol(char symbol);
    void writeIdentifier(const std::string& identifier);
    void writeIntegerConstant(int16_t intConst);
    void writeStringConstant(const std::string& stringConst);
    void beginElement(const std::string& name);
    void endElement();

    // Closes the output file.
    void close();

    // Returns whether the output file has been closed.
    bool isClosed() const
    {
        return m_closed;
    }

private:
    std::filesystem::path   m_filename;
    std::ofstream           m_file;
    std::stack<std::string> m_elementNames;
    std::string             m_indent;
    bool                    m_closed = false;
};
}  // namespace n2t

#endif
