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
