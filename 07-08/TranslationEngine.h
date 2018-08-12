#ifndef TRANSLATION_ENGINE_H
#define TRANSLATION_ENGINE_H

#include "CodeWriter.h"
#include "VmTypes.h"

#include <filesystem>
#include <vector>

namespace n2t
{
class TranslationEngine
{
public:
    enum class WriteInit : bool
    {
        False = false,
        True  = true
    };

    TranslationEngine(PathList inputFilenames, std::filesystem::path outputFilename, WriteInit writeInit);

    void translate();

private:
    PathList   m_inputFilenames;
    CodeWriter m_codeWriter;
};
}  // namespace n2t

#endif
