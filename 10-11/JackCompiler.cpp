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
#include "JackTypes.h"

#include <cxxopts.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
[[nodiscard]] n2t::PathList findInputFiles(const std::filesystem::path& inputPath)
{
    n2t::PathList inputFilenames;

    if (std::filesystem::is_directory(inputPath))
    {
        for (const auto& entry : std::filesystem::directory_iterator(inputPath))
        {
            const auto& path = entry.path();
            if (std::filesystem::is_regular_file(path) && (path.extension() == ".jack"))
            {
                inputFilenames.push_back(path);
            }
        }

        if (inputFilenames.empty())
        {
            throw std::invalid_argument{
                fmt::format("Input directory ({}) does not contain Jack files", inputPath.string())};
        }
    }
    else if (std::filesystem::is_regular_file(inputPath))
    {
        if (inputPath.extension() != ".jack")
        {
            throw std::invalid_argument{fmt::format("Input file ({}) is not a Jack file", inputPath.string())};
        }
        inputFilenames.push_back(inputPath);
    }
    else
    {
        throw std::invalid_argument{fmt::format("Input path ({}) is not a file nor a directory", inputPath.string())};
    }

    return inputFilenames;
}
}  // namespace

int main(int argc, char* argv[])
{
    int result = EXIT_SUCCESS;

    const std::filesystem::path programPath{*argv};
    cxxopts::Options            options{programPath.filename(), "Jack Compiler"};

    try
    {
        /*
         * Parse command line options
         */

        const int maxThreads      = std::max<int>(std::thread::hardware_concurrency(), 1);
        auto      numJobs         = maxThreads;
        bool      outputParseTree = false;

        options.show_positional_help();

        // clang-format off
        options.add_options()
            ("help", "Display this help message")
            ("j,jobs", "Run 'arg' jobs in parallel", cxxopts::value<int>(numJobs)->default_value(std::to_string(maxThreads)))
            ("t,parse-tree", "Output parse tree in XML format", cxxopts::value<bool>(outputParseTree));

        options.add_options("Positional")
            ("input-path", "Input Jack file/directory", cxxopts::value<std::vector<std::string>>());
        // clang-format on

        options.parse_positional("input-path");

        const auto optionsMap = options.parse(argc, argv);

        if (optionsMap.count("help") != 0)
        {
            std::cout << options.help() << '\n';
            return EXIT_SUCCESS;
        }

        /*
         * Find and validate input filenames
         */

        if (numJobs <= 0)
        {
            throw cxxopts::OptionParseException{fmt::format("Option 'jobs' has an invalid argument '{}'", numJobs)};
        }

        const auto inputPathCount = optionsMap.count("input-path");
        if (inputPathCount == 0)
        {
            throw cxxopts::option_required_exception{"input-path"};
        }
        if (inputPathCount != 1)
        {
            throw cxxopts::OptionParseException{"Option 'input-path' is specified more than once"};
        }

        const std::filesystem::path inputPath{optionsMap["input-path"].as<std::vector<std::string>>().front()};
        if (!std::filesystem::exists(inputPath))
        {
            throw std::invalid_argument{fmt::format("Input path ({}) does not exist", inputPath.string())};
        }

        const auto inputFilenames = findInputFiles(inputPath);

        /*
         * Execute compilation tasks
         */

        n2t::CompilationOptions compileOptions;
        compileOptions.outputParseTree = outputParseTree;

        const auto numFiles = inputFilenames.size();
        const auto numTasks = std::min(static_cast<n2t::PathList::size_type>(numJobs), numFiles);
        const auto taskSize = numFiles / numTasks;

        std::vector<n2t::CompilationTask> tasks;
        tasks.reserve(numTasks - 1);
        auto first = inputFilenames.cbegin();
        for (auto task = decltype(numTasks){0}; task < (numTasks - 1); ++task)
        {
            auto last = first + taskSize;
            tasks.emplace_back(first, last, compileOptions, &result);
            first = last;
        }
        n2t::CompilationTask::compile(first, inputFilenames.cend(), compileOptions);
    }
    catch (const cxxopts::OptionException& ex)
    {
        result = EXIT_FAILURE;
        std::cerr << "ERROR: " << ex.what() << "\n\n";
        std::cout << options.help() << '\n';
    }
    catch (const std::exception& ex)
    {
        result = EXIT_FAILURE;
        std::cerr << "ERROR: " << ex.what() << '\n';
    }

    return result;
}
