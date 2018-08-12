#include "SymbolTable.h"

#include <algorithm>
#include <cassert>

n2t::SymbolTable::SymbolTable() : m_nextIndices()
{
}

void n2t::SymbolTable::startSubroutine()
{
    m_subroutineTable.clear();
    m_nextIndices[JackUtil::toUnderType(VariableKind::Argument)] = 0;
    m_nextIndices[JackUtil::toUnderType(VariableKind::Local)]    = 0;
}

void n2t::SymbolTable::define(const std::string& name, const std::string& type, VariableKind kind)
{
    HashTable&     table = getHashTable(kind);
    HashTableEntry entry(type, kind, getNextIndex(kind));
    JACK_THROW_COND(table.emplace(name, std::move(entry)).second,
                    "Identifier with name (" + name + ") already defined in the current scope");
}

unsigned int n2t::SymbolTable::varCount(VariableKind kind) const
{
    const HashTable& table = getHashTable(kind);

    // clang-format off
    return std::count_if(table.begin(),
                         table.end(),
                         [kind](const auto& entry)
                         {
                             return (entry.second.kind == kind);
                         });
    // clang-format on
}

const std::string& n2t::SymbolTable::typeOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    JACK_THROW_COND(entry.kind != VariableKind::None, "Identifier (" + name + ") not defined in the current scope");

    return entry.type;
}

n2t::VariableKind n2t::SymbolTable::kindOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    return entry.kind;
}

unsigned int n2t::SymbolTable::indexOf(const std::string& name) const
{
    const HashTableEntry& entry = getHashTableEntry(name);
    JACK_THROW_COND(entry.kind != VariableKind::None, "Identifier (" + name + ") not defined in the current scope");

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

unsigned int n2t::SymbolTable::getNextIndex(VariableKind kind)
{
    assert(kind < VariableKind::Count);
    return m_nextIndices[JackUtil::toUnderType(kind)]++;
}
