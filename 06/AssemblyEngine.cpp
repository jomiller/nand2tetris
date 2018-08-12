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

#include "AssemblyEngine.h"

#include "AsmUtil.h"
#include "Code.h"
#include "Parser.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <locale>
#include <utility>

n2t::AssemblyEngine::AssemblyEngine(std::filesystem::path inputFilename, std::filesystem::path outputFilename) :
    m_inputFilename(std::move(inputFilename)),
    m_outputFilename(std::move(outputFilename))
{
}

n2t::AssemblyEngine::~AssemblyEngine() noexcept
{
    try
    {
        if (!m_assembled)
        {
            std::filesystem::remove(m_outputFilename);
        }
    }
    catch (...)
    {
    }
}

void n2t::AssemblyEngine::assemble()
{
    ASM_THROW_COND(!m_assembled, "Input file (" + m_inputFilename.filename().string() + ") has already been assembled");
    buildSymbolTable();
    generateCode();
    m_assembled = true;
}

void n2t::AssemblyEngine::buildSymbolTable()
{
    Parser symbolParser(m_inputFilename.string());

    try
    {
        // ROM address to advance when a C-instruction or an A-instruction is encountered,
        // but does not change when a label pseudocommand or a comment is encountered
        int16_t nextRomAddress = 0;

        while (symbolParser.advance())
        {
            const CommandType commandType = symbolParser.commandType();
            if ((commandType == CommandType::A) || (commandType == CommandType::C))
            {
                ++nextRomAddress;
            }
            else if (commandType == CommandType::L)
            {
                const std::string& symbol = symbolParser.symbol();
                ASM_THROW_COND(!std::isdigit(symbol.front(), std::locale()),
                               "Symbol (" + symbol + ") begins with a digit in label command");

                // associate the symbol with the ROM address that will store the next command in the program
                m_symbolTable.addEntry(symbol, nextRomAddress);
            }
        }
    }
    catch (const std::exception& e)
    {
        AsmUtil::throwUncond(e.what(), m_inputFilename.filename().string(), symbolParser.lineNumber());
    }
}

void n2t::AssemblyEngine::generateCode()
{
    Parser codeParser(m_inputFilename.string());

    std::ofstream outputFile(m_outputFilename.string().data());
    AsmUtil::throwCond<std::runtime_error>(outputFile.good(),
                                           "Could not open output file (" + m_outputFilename.string() + ")");

    try
    {
        // starting RAM memory address for mapped variables
        int16_t nextRamAddress = 0x0010;

        while (codeParser.advance())
        {
            const CommandType commandType = codeParser.commandType();
            if (commandType == CommandType::A)
            {
                const std::string& symbol = codeParser.symbol();
                const bool         digits = std::all_of(symbol.begin() + 1, symbol.end(), boost::algorithm::is_digit());
                int16_t            targetAddress = 0;

                if (std::isdigit(symbol.front(), std::locale()))
                {
                    if (digits)
                    {
                        try
                        {
                            targetAddress = boost::lexical_cast<int16_t>(symbol);
                        }
                        catch (const boost::bad_lexical_cast&)
                        {
                            AsmUtil::throwUncond("Address (" + symbol + ") is too large in addressing instruction");
                        }
                    }
                    else
                    {
                        AsmUtil::throwUncond("Symbol (" + symbol + ") begins with a digit in addressing instruction");
                    }
                }
                else
                {
                    ASM_THROW_COND((symbol.front() != '-') || (symbol.length() <= 1) || !digits,
                                   "Address (" + symbol + ") is negative in addressing instruction");

                    // this is a symbolic A-instruction, i.e. @Xxx where Xxx is a symbol rather than an integer
                    if (m_symbolTable.contains(symbol))
                    {
                        // replace the symbol with its associated address
                        targetAddress = m_symbolTable.getAddress(symbol);
                    }
                    else
                    {
                        // the symbol represents a new variable
                        // associate the variable with the next available RAM address
                        m_symbolTable.addEntry(symbol, nextRamAddress);
                        targetAddress = nextRamAddress;
                        ++nextRamAddress;
                    }
                }

                outputFile << "0" << std::bitset<15>(targetAddress) << '\n';
            }
            else if (commandType == CommandType::C)
            {
                const uint16_t compCode = Code::comp(codeParser.comp());  // 7 bits
                const uint16_t destCode = Code::dest(codeParser.dest());  // 3 bits
                const uint16_t jumpCode = Code::jump(codeParser.jump());  // 3 bits

                const uint16_t instruction = (uint16_t(0b111) << 13) | (compCode << 6) | (destCode << 3) | jumpCode;

                outputFile << std::bitset<16>(instruction) << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        AsmUtil::throwUncond(e.what(), m_inputFilename.filename().string(), codeParser.lineNumber());
    }
}
