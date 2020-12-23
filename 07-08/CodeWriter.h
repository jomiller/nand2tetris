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

#ifndef N2T_CODE_WRITER_H
#define N2T_CODE_WRITER_H

#include "VmTypes.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace n2t
{
class CodeWriter
{
public:
    // Opens the output file and gets ready to write into it.
    explicit CodeWriter(std::filesystem::path filename);

    CodeWriter(const CodeWriter&) = delete;
    CodeWriter(CodeWriter&&)      = delete;

    CodeWriter& operator=(const CodeWriter&) = delete;
    CodeWriter& operator=(CodeWriter&&) = delete;

    ~CodeWriter() noexcept;

    // Informs the code writer that the translation of a new VM file is started.
    void setFilename(std::string_view inputFilename);

    // Writes assembly code that effects the VM initialization, also called bootstrap code.
    void writeInit();

    // Writes the assembly code that is the translation of the given arithmetic command.
    void writeArithmetic(const std::string& command);

    // Writes the assembly code that is the translation of the given command, where command is either Push or
    // Pop.
    void writePushPop(CommandType command, const std::string& segment, int16_t index);

    // Writes assembly code that effects the label command.
    void writeLabel(const std::string& label);

    // Writes assembly code that effects the goto command.
    void writeGoto(const std::string& label);

    // Writes assembly code that effects the if-goto command.
    void writeIf(const std::string& label);

    // Writes assembly code that effects the function command.
    void writeFunction(const std::string& functionName, int16_t numLocals);

    // Writes assembly code that effects the return command.
    void writeReturn();

    // Writes assembly code that effects the call command.
    void writeCall(const std::string& functionName, int16_t numArguments);

    // Closes the output file.
    void close();

    // Returns whether the output file has been closed.
    [[nodiscard]] bool isClosed() const
    {
        return m_closed;
    }

private:
    struct FunctionInfo
    {
        FunctionInfo() = default;

        FunctionInfo(std::string n, int16_t p) : name{std::move(n)}, numParameters{p}
        {
        }

        std::string name;
        int16_t     numParameters = 0;
    };

    struct FunctionCallInfo
    {
        FunctionCallInfo() = default;

        FunctionCallInfo(std::string n, int16_t a) : name{std::move(n)}, numArguments{a}
        {
        }

        [[nodiscard]] bool operator<(const FunctionCallInfo& rhs) const
        {
            if (name != rhs.name)
            {
                return (name < rhs.name);  // NOLINT(modernize-use-nullptr)
            }
            return (numArguments < rhs.numArguments);
        }

        std::string name;
        int16_t     numArguments = 0;
    };

    void                       pushFromD();
    void                       popToD();
    [[nodiscard]] unsigned int getNextLabelId();
    [[nodiscard]] std::string  makeLabel(const std::string& label) const;
    [[nodiscard]] std::string  makeNumberedLabel(std::string_view label);
    void                       validateFunction();
    void                       validateFunctionCalls() const;

    std::filesystem::path                    m_outputFilename;
    std::ofstream                            m_file;
    std::string                              m_currentInputFilename;
    FunctionInfo                             m_currentFunction;
    std::unordered_set<std::string>          m_labels;
    std::unordered_set<std::string>          m_gotoDestinations;
    std::unordered_map<std::string, int16_t> m_definedFunctions;
    std::set<FunctionCallInfo>               m_calledFunctions;
    unsigned int                             m_nextLabelId = 0;
    bool                                     m_closed      = false;
};
}  // namespace n2t

#endif
