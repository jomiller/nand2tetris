#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <cstdint>
#include <map>
#include <string>

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
    bool contains(const std::string& symbol) const;

    // Returns the address associated with the symbol.
    int16_t getAddress(const std::string& symbol) const;

private:
    std::map<std::string, int16_t> m_table;
};
}  // namespace n2t

#endif
