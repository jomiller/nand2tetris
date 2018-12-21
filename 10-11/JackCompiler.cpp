/*
 * This file is part of Nand2Tetris.
 *
 * Copyright (c) 2013-2018 Jonathan Miller
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

#include <boost/program_options.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace
{
namespace po = boost::program_options;

void displayHelp(const std::filesystem::path& programPath, const po::options_description& options)
{
    const std::string programName = programPath.filename().string();
    std::cout << "Usage: " << programName << " [options] <filename>.jack\n";
    std::cout << "       " << programName << " [options] <directory>\n\n";
    std::cout << options << '\n';
}

po::variables_map parseOptions(int argc, char* argv[], const po::options_description& visibleOptions)
{
    po::options_description hiddenOptions("Hidden Options");
    // clang-format off
    hiddenOptions.add_options()
        ("input-path", po::value<std::string>()->required(), "Input file/directory");
    // clang-format on

    po::options_description cmdlineOptions;
    cmdlineOptions.add(visibleOptions).add(hiddenOptions);

    po::positional_options_description positionalOptions;
    positionalOptions.add("input-path", 1);

    po::variables_map optionsMap;

    po::store(po::command_line_parser(argc, argv).options(cmdlineOptions).positional(positionalOptions).run(),
              optionsMap);

    return optionsMap;
}

n2t::PathList findInputFiles(const std::filesystem::path& inputPath)
{
    n2t::PathList inputFilenames;

    if (std::filesystem::is_directory(inputPath))
    {
        for (const auto& entry : std::filesystem::directory_iterator(inputPath))
        {
            const std::filesystem::path& path = entry.path();
            if (std::filesystem::is_regular_file(path) && (path.extension() == ".jack"))
            {
                inputFilenames.push_back(path);
            }
        }

        if (inputFilenames.empty())
        {
            throw std::invalid_argument("Input directory (" + inputPath.string() + ") does not contain Jack files");
        }
    }
    else if (std::filesystem::is_regular_file(inputPath))
    {
        if (inputPath.extension() != ".jack")
        {
            throw std::invalid_argument("Input file (" + inputPath.string() + ") is not a Jack file");
        }
        inputFilenames.push_back(inputPath);
    }
    else
    {
        throw std::invalid_argument("Input path (" + inputPath.string() + ") is not a file nor a directory");
    }

    return inputFilenames;
}
}  // namespace

int main(int argc, char* argv[])
{
    int result = EXIT_SUCCESS;

    const std::filesystem::path programPath(argv[0]);
    po::options_description     visibleOptions("Options");

    try
    {
        /*
         * Parse command line options
         */

        const int maxThreads      = std::max<int>(std::thread::hardware_concurrency(), 1);
        int       numJobs         = maxThreads;
        bool      outputParseTree = false;

        // clang-format off
        auto validateJobs =
            [](int value)
            {
                if (value <= 0)
                {
                    throw po::validation_error(
                        po::validation_error::invalid_option_value, "j", std::to_string(value));
                }
            };

        visibleOptions.add_options()
            ("help", "Display this help message")
            ("jobs,j", po::value<int>(&numJobs)->default_value(maxThreads)->notifier(validateJobs),
                "Run 'arg' jobs in parallel")
            ("parse-tree,t", po::bool_switch(&outputParseTree), "Output parse tree in XML format");
        // clang-format on

        po::variables_map optionsMap = parseOptions(argc, argv, visibleOptions);

        if (optionsMap.count("help") > 0)
        {
            displayHelp(programPath, visibleOptions);
            return EXIT_SUCCESS;
        }

        po::notify(optionsMap);

        /*
         * Find and validate input filenames
         */

        const std::filesystem::path inputPath(optionsMap["input-path"].as<std::string>());
        if (!std::filesystem::exists(inputPath))
        {
            throw std::invalid_argument("Input path (" + inputPath.string() + ") does not exist");
        }

        const n2t::PathList inputFilenames = findInputFiles(inputPath);

        /*
         * Execute compilation tasks
         */

        n2t::CompilationOptions options;
        options.outputParseTree = outputParseTree;

        const auto numFiles = inputFilenames.size();
        const auto numTasks = std::min(static_cast<std::remove_const_t<decltype(numFiles)>>(numJobs), numFiles);
        const auto taskSize = numFiles / numTasks;

        std::vector<n2t::CompilationTask> tasks;
        tasks.reserve(numTasks - 1);
        auto first = inputFilenames.cbegin();
        for (auto task = decltype(numTasks){0}; task < (numTasks - 1); ++task)
        {
            auto last = first + taskSize;
            tasks.emplace_back(first, last, options);
            first = last;
        }
        n2t::CompilationTask::compile(first, inputFilenames.cend(), options);
    }
    catch (const po::required_option&)
    {
        result = EXIT_FAILURE;
        std::cerr << "ERROR: No input path\n\n";
        displayHelp(programPath, visibleOptions);
    }
    catch (const po::error& e)
    {
        result = EXIT_FAILURE;
        std::cerr << "ERROR: " << e.what() << "\n\n";
        displayHelp(programPath, visibleOptions);
    }
    catch (const std::exception& e)
    {
        result = EXIT_FAILURE;
        std::cerr << "ERROR: " << e.what() << '\n';
    }

    return result;
}
