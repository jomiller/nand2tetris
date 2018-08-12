#include "SymbolTable.h"

#include "AsmUtil.h"

n2t::SymbolTable::SymbolTable()
{
    // initialize the table with the predefined symbols
    m_table["SP"]   = 0x0000;
    m_table["LCL"]  = 0x0001;
    m_table["ARG"]  = 0x0002;
    m_table["THIS"] = 0x0003;
    m_table["THAT"] = 0x0004;
    for (int16_t i = 0; i < 16; ++i)
    {
        m_table["R" + std::to_string(i)] = i;
    }
    m_table["SCREEN"] = 0x4000;
    m_table["KBD"]    = 0x6000;
}

void n2t::SymbolTable::addEntry(const std::string& symbol, int16_t address)
{
    ASM_THROW_COND(m_table.emplace(symbol, address).second, "Symbol (" + symbol + ") already exists in the table");
}

bool n2t::SymbolTable::contains(const std::string& symbol) const
{
    return (m_table.find(symbol) != m_table.end());
}

int16_t n2t::SymbolTable::getAddress(const std::string& symbol) const
{
    const auto iter = m_table.find(symbol);
    ASM_THROW_COND(iter != m_table.end(), "Symbol (" + symbol + ") not found in the table");

    return iter->second;
}
