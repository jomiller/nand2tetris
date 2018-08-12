#include "CompilationTask.h"

#include "CompilationEngine.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

n2t::CompilationTask::CompilationTask(const PathList& inputFilenames, CompilationOptions options, int* result) :
    CompilationTask(inputFilenames.cbegin(), inputFilenames.cend(), options, result)
{
}

n2t::CompilationTask::CompilationTask(PathList::const_iterator firstFilename,
                                      PathList::const_iterator lastFilename,
                                      CompilationOptions       options,
                                      int*                     result) :
    m_future(std::async(std::launch::async, compileFileRange, firstFilename, lastFilename, options)),
    m_result(result)
{
}

n2t::CompilationTask::~CompilationTask() noexcept
{
    try
    {
        if (m_future.valid())
        {
            m_future.get();
        }
    }
    catch (const std::exception& e)
    {
        if (m_result)
        {
            *m_result = EXIT_FAILURE;
        }
        std::cerr << "ERROR: " << e.what() << '\n';
    }
}

void n2t::CompilationTask::compile(const PathList& inputFilenames, CompilationOptions options)
{
    compileFileRange(inputFilenames.cbegin(), inputFilenames.cend(), options);
}

void n2t::CompilationTask::compile(PathList::const_iterator firstFilename,
                                   PathList::const_iterator lastFilename,
                                   CompilationOptions       options)
{
    compileFileRange(firstFilename, lastFilename, options);
}

void n2t::CompilationTask::compileFileRange(PathList::const_iterator firstFilename,
                                            PathList::const_iterator lastFilename,
                                            CompilationOptions       options)
{
    // clang-format off
    std::for_each(firstFilename,
                  lastFilename,
                  [options](const auto& filename)
                  {
                      compileFile(filename, options);
                  });
    // clang-format on
}

void n2t::CompilationTask::compileFile(const std::filesystem::path& inputFilename, CompilationOptions options)
{
    std::filesystem::path vmOutputFilename = inputFilename;
    vmOutputFilename.replace_extension(".vm");
    std::filesystem::path xmlOutputFilename;
    if (options.outputParseTree)
    {
        xmlOutputFilename = inputFilename;
        xmlOutputFilename.replace_extension(".xml");
    }

    CompilationEngine engine(inputFilename, std::move(vmOutputFilename), xmlOutputFilename);
    engine.compileClass();
}
