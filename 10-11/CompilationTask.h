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
