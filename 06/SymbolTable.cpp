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

#include "SymbolTable.h"

#include <Util.h>

#include <fmt/format.h>

n2t::SymbolTable::SymbolTable()
{
    // initialize the table with the predefined symbols
    const int16_t numNamedRamLocations = 16;
    for (int16_t i = 0; i < numNamedRamLocations; ++i)
    {
        m_table[fmt::format("R{}", i)] = i;
    }

    m_table["SP"]     = 0x0000;
    m_table["LCL"]    = 0x0001;
    m_table["ARG"]    = 0x0002;
    m_table["THIS"]   = 0x0003;
    m_table["THAT"]   = 0x0004;
    m_table["SCREEN"] = 0x4000;  // NOLINT(readability-magic-numbers)
    m_table["KBD"]    = 0x6000;  // NOLINT(readability-magic-numbers)
}

void n2t::SymbolTable::addEntry(const std::string& symbol, int16_t address)
{
    throwIfNot(m_table.emplace(symbol, address).second, "Symbol ({}) already exists in the table", symbol);
}

bool n2t::SymbolTable::contains(const std::string& symbol) const
{
    return (m_table.find(symbol) != m_table.end());
}

int16_t n2t::SymbolTable::getAddress(const std::string& symbol) const
{
    const auto iter = m_table.find(symbol);
    throwIfNot(iter != m_table.end(), "Symbol ({}) not found in the table", symbol);

    return iter->second;
}
