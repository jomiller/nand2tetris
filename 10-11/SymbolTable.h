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

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "JackTypes.h"
#include "JackUtil.h"

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

namespace n2t
{
class SymbolTable
{
public:
    // Creates a new empty symbol table.
    SymbolTable() = default;

    // Starts a new subroutine scope (i.e., resets the subroutine's symbol table).
    void startSubroutine();

    // Defines a new identifier of a given name, type, and kind and assigns it a running index.
    void define(const std::string& name, const std::string& type, VariableKind kind);

    // Returns the number of variables of the given kind already defined in the current scope.
    [[nodiscard]] int16_t varCount(VariableKind kind) const;

    // Returns the type of the named identifier in the current scope.
    [[nodiscard]] const std::string& typeOf(const std::string& name) const;

    // Returns the kind of the named identifier in the current scope.
    // If the identifier is unknown in the current scope, returns None.
    [[nodiscard]] VariableKind kindOf(const std::string& name) const;

    // Returns the index assigned to the named identifier.
    [[nodiscard]] int16_t indexOf(const std::string& name) const;

private:
    struct HashTableEntry
    {
        HashTableEntry() = default;

        HashTableEntry(std::string t, VariableKind k, int16_t i) : type{std::move(t)}, kind{k}, index{i}
        {
        }

        std::string  type;
        VariableKind kind  = VariableKind::None;
        int16_t      index = 0;
    };

    using HashTable = std::unordered_map<std::string, HashTableEntry>;

    HashTable&                          getHashTable(VariableKind kind);
    [[nodiscard]] const HashTable&      getHashTable(VariableKind kind) const;
    [[nodiscard]] const HashTableEntry& getHashTableEntry(const std::string& name) const;
    int16_t                             getNextVarIndex(VariableKind kind);

    HashTable                                                            m_classTable;
    HashTable                                                            m_subroutineTable;
    std::array<int16_t, JackUtil::toUnderlyingType(VariableKind::Count)> m_nextVarIndices = {};
};
}  // namespace n2t

#endif
