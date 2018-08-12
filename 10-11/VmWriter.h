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

#ifndef VM_WRITER_H
#define VM_WRITER_H

#include "VmTypes.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace n2t
{
class VmWriter
{
public:
    // Creates a new file and prepares it for writing.
    explicit VmWriter(std::filesystem::path filename);

    VmWriter(const VmWriter&) = delete;
    VmWriter(VmWriter&&)      = delete;

    VmWriter& operator=(const VmWriter&) = delete;
    VmWriter& operator=(VmWriter&&) = delete;

    ~VmWriter() noexcept;

    // Writes a VM push command.
    void writePush(SegmentType segment, int16_t index);

    // Writes a VM pop command.
    void writePop(SegmentType segment, int16_t index);

    // Writes a VM arithmetic command.
    void writeArithmetic(ArithmeticCommand command);

    // Writes a VM label command.
    void writeLabel(const std::string& label);

    // Writes a VM goto command.
    void writeGoto(const std::string& label);

    // Writes a VM if-goto command.
    void writeIf(const std::string& label);

    // Writes a VM function command.
    void writeFunction(const std::string& functionName, unsigned int numLocals);

    // Writes a VM return command.
    void writeReturn();

    // Writes a VM call command.
    void writeCall(const std::string& functionName, unsigned int numArguments);

    // Closes the output file.
    void close();

    // Returns whether the output file has been closed.
    bool isClosed() const
    {
        return m_closed;
    }

private:
    static const std::string& toString(SegmentType segment);

    std::filesystem::path m_filename;
    std::ofstream         m_file;
    bool                  m_closed = false;
};
}  // namespace n2t

#endif
