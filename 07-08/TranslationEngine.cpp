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

#include "TranslationEngine.h"

#include "Parser.h"
#include "VmUtil.h"

#include <string>
#include <utility>

n2t::TranslationEngine::TranslationEngine(PathList              inputFilenames,
                                          std::filesystem::path outputFilename,
                                          WriteInit             writeInit) :
    m_inputFilenames(std::move(inputFilenames)),
    m_codeWriter(std::move(outputFilename))
{
    if (writeInit == WriteInit::True)
    {
        m_codeWriter.writeInit();
    }
}

void n2t::TranslationEngine::translate()
{
    N2T_VM_THROW_COND(!m_codeWriter.isClosed(), "Input files have already been translated");

    for (const auto& path : m_inputFilenames)
    {
        const std::string inputFilename = path.filename().string();
        Parser            parser(path.string());

        try
        {
            m_codeWriter.setFilename(inputFilename);
            while (parser.advance())
            {
                const CommandType commandType = parser.commandType();
                switch (commandType)
                {
                    case CommandType::Arithmetic:
                        m_codeWriter.writeArithmetic(parser.arg1());
                        break;

                    case CommandType::Push:
                    case CommandType::Pop:
                        m_codeWriter.writePushPop(commandType, parser.arg1(), parser.arg2());
                        break;

                    case CommandType::Label:
                        m_codeWriter.writeLabel(parser.arg1());
                        break;

                    case CommandType::Goto:
                        m_codeWriter.writeGoto(parser.arg1());
                        break;

                    case CommandType::If:
                        m_codeWriter.writeIf(parser.arg1());
                        break;

                    case CommandType::Function:
                        m_codeWriter.writeFunction(parser.arg1(), parser.arg2());
                        break;

                    case CommandType::Return:
                        m_codeWriter.writeReturn();
                        break;

                    case CommandType::Call:
                        m_codeWriter.writeCall(parser.arg1(), parser.arg2());
                        break;

                    default:
                        VmUtil::throwUncond("Unsupported command type (" + VmUtil::toString(commandType) + ")");
                }
            }
        }
        catch (const std::exception& e)
        {
            VmUtil::throwUncond(e.what(), inputFilename, parser.lineNumber());
        }
    }

    m_codeWriter.close();
}
