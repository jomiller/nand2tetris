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

#include "JackAssert.h"

#include <algorithm>
#include <limits>

n2t::SymbolTable::SymbolTable() : m_nextVarIndices()
{
}

void n2t::SymbolTable::startSubroutine()
{
    m_subroutineTable.clear();
    m_nextVarIndices[JackUtil::toUnderlyingType(VariableKind::Argument)] = 0;
    m_nextVarIndices[JackUtil::toUnderlyingType(VariableKind::Local)]    = 0;
}

void n2t::SymbolTable::define(const std::string& name, const std::string& type, VariableKind kind)
{
    HashTable&     table = getHashTable(kind);
    HashTableEntry entry(type, kind, getNextVarIndex(kind));
    N2T_JACK_THROW_COND(table.emplace(name, std::move(entry)).second,
                        "Identifier with name (" + name + ") already defined in the current scope");
}

int16_t n2t::SymbolTable::varCount(VariableKind kind) const
{
    const HashTable& table = getHashTable(kind);

    // clang-format off
    const auto count = std::count_if(table.begin(),
                                     table.end(),
                                     [kind](const auto& entry)
                                     {
                                         return (entry.second.kind == kind);
                                     });
    // clang-format on

    return static_cast<int16_t>(count);
}

const std::string& n2t::SymbolTable::typeOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    N2T_JACK_THROW_COND(entry.kind != VariableKind::None, "Identifier (" + name + ") not defined in the current scope");

    return entry.type;
}

n2t::VariableKind n2t::SymbolTable::kindOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    return entry.kind;
}

int16_t n2t::SymbolTable::indexOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    N2T_JACK_THROW_COND(entry.kind != VariableKind::None, "Identifier (" + name + ") not defined in the current scope");

    return entry.index;
}

n2t::SymbolTable::HashTable& n2t::SymbolTable::getHashTable(VariableKind kind)
{
    if ((kind == VariableKind::Static) || (kind == VariableKind::Field))
    {
        return m_classTable;
    }
    return m_subroutineTable;
}

const n2t::SymbolTable::HashTable& n2t::SymbolTable::getHashTable(VariableKind kind) const
{
    if ((kind == VariableKind::Static) || (kind == VariableKind::Field))
    {
        return m_classTable;
    }
    return m_subroutineTable;
}

const n2t::SymbolTable::HashTableEntry& n2t::SymbolTable::getHashTableEntry(const std::string& name) const
{
    auto entry = m_subroutineTable.find(name);
    if (entry != m_subroutineTable.end())
    {
        return entry->second;
    }

    entry = m_classTable.find(name);
    if (entry != m_classTable.end())
    {
        return entry->second;
    }

    static const HashTableEntry none;
    return none;
}

int16_t n2t::SymbolTable::getNextVarIndex(VariableKind kind)
{
    N2T_JACK_ASSERT(kind < VariableKind::Count);

    const int16_t maxVarIndex = std::numeric_limits<int16_t>::max() - 1;

    int16_t* nextVarIndex = &m_nextVarIndices.at(JackUtil::toUnderlyingType(kind));

    N2T_JACK_THROW_COND(*nextVarIndex < maxVarIndex,
                        "Variable count for kind (" + JackUtil::toString(kind) + ") exceeds the limit (" +
                            std::to_string(maxVarIndex + 1) + ")");

    return (*nextVarIndex)++;
}
