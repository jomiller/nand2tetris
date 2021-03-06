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

#ifndef N2T_SYMBOL_TABLE_H
#define N2T_SYMBOL_TABLE_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace n2t
{
class SymbolTable
{
public:
    // Creates a new symbol table and adds the predefined symbols to it.
    SymbolTable();

    // Adds the pair (symbol, address) to the table.
    void addEntry(const std::string& symbol, int16_t address);

    // Does the symbol table contain the given symbol?
    [[nodiscard]] bool contains(const std::string& symbol) const;

    // Returns the address associated with the symbol.
    [[nodiscard]] int16_t getAddress(const std::string& symbol) const;

private:
    std::unordered_map<std::string, int16_t> m_table;
};
}  // namespace n2t

#endif
