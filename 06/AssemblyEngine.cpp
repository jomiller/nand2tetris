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
#include <iterator>
#include <limits>
#include <locale>
#include <utility>

n2t::AssemblyEngine::AssemblyEngine(std::filesystem::path inputFilename, std::filesystem::path outputFilename) :
    m_inputFilename{std::move(inputFilename)},
    m_outputFilename{std::move(outputFilename)}
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
    AsmUtil::throwCond(!m_assembled, "Input file ({}) has already been assembled", m_inputFilename.filename().string());
    buildSymbolTable();
    generateCode();
    m_assembled = true;
}

void n2t::AssemblyEngine::buildSymbolTable()
{
    Parser symbolParser{m_inputFilename.string()};

    try
    {
        const int16_t maxRomAddress = std::numeric_limits<int16_t>::max() - 1;

        // ROM address to advance when a C-instruction or an A-instruction is encountered,
        // but does not change when a label pseudocommand or a comment is encountered
        int16_t nextRomAddress = 0;

        while (symbolParser.advance())
        {
            const auto commandType = symbolParser.commandType();
            if ((commandType == CommandType::A) || (commandType == CommandType::C))
            {
                AsmUtil::throwCond(
                    nextRomAddress < maxRomAddress, "Instruction count exceeds the limit ({})", maxRomAddress + 1);

                ++nextRomAddress;
            }
            else if (commandType == CommandType::L)
            {
                const auto& symbol = symbolParser.symbol();
                AsmUtil::throwCond(!std::isdigit(symbol.front(), std::locale{}),
                                   "Symbol ({}) begins with a digit in label command",
                                   symbol);

                // associate the symbol with the ROM address that will store the next command in the program
                m_symbolTable.addEntry(symbol, nextRomAddress);
            }
        }
    }
    catch (const std::exception& ex)
    {
        AsmUtil::throwUncond({m_inputFilename.filename().string(), symbolParser.lineNumber()}, ex.what());
    }
}

void n2t::AssemblyEngine::generateCode()
{
    Parser codeParser{m_inputFilename.string()};

    std::ofstream outputFile{m_outputFilename.string().data()};
    AsmUtil::throwCond<std::runtime_error>(
        outputFile.good(), "Could not open output file ({})", m_outputFilename.string());

    try
    {
        const int16_t baseRamAddress = 0x0010;
        const int16_t maxRamAddress  = std::numeric_limits<int16_t>::max() - 1;

        int16_t nextRamAddress = baseRamAddress;

        while (codeParser.advance())
        {
            const auto commandType = codeParser.commandType();
            if (commandType == CommandType::A)
            {
                const auto& symbol = codeParser.symbol();
                const auto  digits = std::all_of(std::next(symbol.begin()), symbol.end(), boost::algorithm::is_digit());
                int16_t     targetAddress = 0;

                if (std::isdigit(symbol.front(), std::locale{}))
                {
                    if (digits)
                    {
                        try
                        {
                            targetAddress = boost::lexical_cast<int16_t>(symbol);
                        }
                        catch (const boost::bad_lexical_cast&)
                        {
                            AsmUtil::throwUncond("Address ({}) is too large in addressing instruction", symbol);
                        }
                    }
                    else
                    {
                        AsmUtil::throwUncond("Symbol ({}) begins with a digit in addressing instruction", symbol);
                    }
                }
                else
                {
                    AsmUtil::throwCond((symbol.front() != '-') || (symbol.length() <= 1) || !digits,
                                       "Address ({}) is negative in addressing instruction",
                                       symbol);

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

                        AsmUtil::throwCond(
                            nextRamAddress < maxRamAddress, "Variable count exceeds the limit ({})", maxRamAddress + 1);

                        ++nextRamAddress;
                    }
                }

                // NOLINTNEXTLINE(readability-magic-numbers)
                outputFile << "0" << std::bitset<15>(targetAddress) << '\n';
            }
            else if (commandType == CommandType::C)
            {
                const auto compCode = Code::comp(codeParser.comp());  // 7 bits
                const auto destCode = Code::dest(codeParser.dest());  // 3 bits
                const auto jumpCode = Code::jump(codeParser.jump());  // 3 bits

                const auto instruction =
                    static_cast<uint16_t>((uint16_t{0b111} << 13) | (compCode << 6) | (destCode << 3) | jumpCode);

                // NOLINTNEXTLINE(readability-magic-numbers)
                outputFile << std::bitset<16>(instruction) << '\n';
            }
        }
    }
    catch (const std::exception& ex)
    {
        AsmUtil::throwUncond({m_inputFilename.filename().string(), codeParser.lineNumber()}, ex.what());
    }
}
