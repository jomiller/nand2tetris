#ifndef ASSEMBLY_ENGINE_H
#define ASSEMBLY_ENGINE_H

#include "SymbolTable.h"

#include <filesystem>
#include <string>

namespace n2t
{
class AssemblyEngine
{
public:
    AssemblyEngine(std::filesystem::path inputFilename, std::filesystem::path outputFilename);

    AssemblyEngine(const AssemblyEngine&) = delete;
    AssemblyEngine(AssemblyEngine&&)      = delete;

    AssemblyEngine& operator=(const AssemblyEngine&) = delete;
    AssemblyEngine& operator=(AssemblyEngine&&) = delete;

    ~AssemblyEngine() noexcept;

    void assemble();

private:
    // Builds the symbol table without generating any code.
    void buildSymbolTable();

    // Replaces each symbol with its corresponding meaning (numeric address)
    // and generates the final binary code.
    void generateCode();

    std::filesystem::path m_inputFilename;
    std::filesystem::path m_outputFilename;
    SymbolTable           m_symbolTable;
    bool                  m_assembled = false;
};
}  // namespace n2t

#endif
