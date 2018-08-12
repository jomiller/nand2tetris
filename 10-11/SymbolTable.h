#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "JackTypes.h"
#include "JackUtil.h"

#include <array>
#include <map>
#include <string>
#include <utility>

namespace n2t
{
class SymbolTable
{
public:
    // Creates a new empty symbol table.
    SymbolTable();

    // Starts a new subroutine scope (i.e., resets the subroutine's symbol table).
    void startSubroutine();

    // Defines a new identifier of a given name, type, and kind and assigns it a running index.
    void define(const std::string& name, const std::string& type, VariableKind kind);

    // Returns the number of variables of the given kind already defined in the current scope.
    unsigned int varCount(VariableKind kind) const;

    // Returns the type of the named identifier in the current scope.
    const std::string& typeOf(const std::string& name) const;

    // Returns the kind of the named identifier in the current scope.
    // If the identifier is unknown in the current scope, returns None.
    VariableKind kindOf(const std::string& name) const;

    // Returns the index assigned to the named identifier.
    unsigned int indexOf(const std::string& name) const;

private:
    struct HashTableEntry
    {
        std::string  type;
        VariableKind kind  = VariableKind::None;
        unsigned int index = 0;

        HashTableEntry() = default;

        HashTableEntry(std::string t, VariableKind k, unsigned int i) : type(std::move(t)), kind(k), index(i)
        {
        }
    };

    using HashTable = std::map<std::string, HashTableEntry>;

    HashTable&            getHashTable(VariableKind kind);
    const HashTable&      getHashTable(VariableKind kind) const;
    const HashTableEntry& getHashTableEntry(const std::string& name) const;
    unsigned int          getNextIndex(VariableKind kind);

    HashTable                                                            m_classTable;
    HashTable                                                            m_subroutineTable;
    std::array<unsigned int, JackUtil::toUnderType(VariableKind::Count)> m_nextIndices;
};
}  // namespace n2t

#endif
