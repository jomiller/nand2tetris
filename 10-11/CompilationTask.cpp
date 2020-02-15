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

#include "CompilationTask.h"

#include "CompilationEngine.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

n2t::CompilationTask::CompilationTask(const PathList& inputFilenames, CompilationOptions options, int* result) :
    CompilationTask{inputFilenames.cbegin(), inputFilenames.cend(), options, result}
{
}

n2t::CompilationTask::CompilationTask(PathList::const_iterator firstFilename,
                                      PathList::const_iterator lastFilename,
                                      CompilationOptions       options,
                                      int*                     result) :
    m_future{std::async(std::launch::async, compileFileRange, firstFilename, lastFilename, options)},
    m_result{result}
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
    catch (const std::exception& ex)
    {
        if (m_result)
        {
            *m_result = EXIT_FAILURE;
        }
        std::cerr << "ERROR: " << ex.what() << '\n';
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

    CompilationEngine engine{inputFilename, std::move(vmOutputFilename), xmlOutputFilename};
    engine.compileClass();
}
