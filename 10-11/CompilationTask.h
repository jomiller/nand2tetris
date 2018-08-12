#ifndef COMPILATION_TASK_H
#define COMPILATION_TASK_H

#include "JackTypes.h"

#include <filesystem>
#include <future>

namespace n2t
{
struct CompilationOptions
{
    bool outputParseTree = false;
};

class CompilationTask
{
public:
    explicit CompilationTask(const PathList& inputFilenames, CompilationOptions options = {}, int* result = nullptr);

    explicit CompilationTask(PathList::const_iterator firstFilename,
                             PathList::const_iterator lastFilename,
                             CompilationOptions       options = {},
                             int*                     result  = nullptr);

    CompilationTask(const CompilationTask&)     = delete;
    CompilationTask(CompilationTask&&) noexcept = default;

    CompilationTask& operator=(const CompilationTask&) = delete;
    CompilationTask& operator=(CompilationTask&&) noexcept = default;

    ~CompilationTask() noexcept;

    static void compile(const PathList& inputFilenames, CompilationOptions options = {});

    static void compile(PathList::const_iterator firstFilename,
                        PathList::const_iterator lastFilename,
                        CompilationOptions       options = {});

private:
    static void compileFileRange(PathList::const_iterator firstFilename,
                                 PathList::const_iterator lastFilename,
                                 CompilationOptions       options);

    static void compileFile(const std::filesystem::path& inputFilename, CompilationOptions options);

    std::future<void> m_future;
    int*              m_result = nullptr;
};
}  // namespace n2t

#endif
