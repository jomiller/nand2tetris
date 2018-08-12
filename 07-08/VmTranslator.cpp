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

#include "TranslationEngine.h"
#include "VmTypes.h"

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
namespace po = boost::program_options;

void displayHelp(const std::filesystem::path& programPath, const po::options_description& options)
{
    const std::string programName = programPath.filename().string();
    std::cout << "Usage: " << programName << " <filename>.vm\n";
    std::cout << "       " << programName << " <directory>\n\n";
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

n2t::PathList findInputFiles(const std::filesystem::path& inputPath,
                             bool                         isInputDirectory,
                             std::filesystem::path&       outputFilename)
{
    n2t::PathList inputFilenames;

    if (isInputDirectory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(inputPath))
        {
            const std::filesystem::path& path = entry.path();
            if (std::filesystem::is_regular_file(path) && (path.extension() == ".vm"))
            {
                inputFilenames.push_back(path);
            }
        }

        if (inputFilenames.empty())
        {
            throw std::invalid_argument("Input directory (" + inputPath.string() + ") does not contain VM files");
        }

        if (outputFilename.empty())
        {
            const std::filesystem::path inputPathAbsolute = std::filesystem::absolute(inputPath);
            if (inputPathAbsolute.stem().empty() || (inputPathAbsolute.stem() == "."))
            {
                // path contains a trailing slash
                outputFilename = inputPathAbsolute.parent_path() / inputPathAbsolute.parent_path().stem();
            }
            else
            {
                // path does not contain a trailing slash
                outputFilename = inputPathAbsolute / inputPathAbsolute.stem();
            }
            outputFilename.replace_extension(".asm");
        }
    }
    else if (std::filesystem::is_regular_file(inputPath))
    {
        if (inputPath.extension() != ".vm")
        {
            throw std::invalid_argument("Input file (" + inputPath.string() + ") is not a VM file");
        }
        inputFilenames.push_back(inputPath);

        if (outputFilename.empty())
        {
            outputFilename = inputPath;
            outputFilename.replace_extension(".asm");
        }
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
    int result = EXIT_FAILURE;

    const std::filesystem::path programPath(argv[0]);
    po::options_description     visibleOptions("Options");

    try
    {
        /*
         * Parse command line options
         */

        std::filesystem::path outputFilename;

        // clang-format off
        visibleOptions.add_options()
            ("help", "Display this help message")
            ("output-file,o", po::value<std::filesystem::path>(&outputFilename), "Output file");
        // clang-format on

        po::variables_map optionsMap = parseOptions(argc, argv, visibleOptions);

        if (optionsMap.count("help") > 0)
        {
            displayHelp(programPath, visibleOptions);
            return EXIT_SUCCESS;
        }

        po::notify(optionsMap);

        /*
         * Find and validate input and output filenames
         */

        const std::filesystem::path inputPath(optionsMap["input-path"].as<std::string>());
        if (!std::filesystem::exists(inputPath))
        {
            throw std::invalid_argument("Input path (" + inputPath.string() + ") does not exist");
        }

        const bool isInputDirectory = std::filesystem::is_directory(inputPath);
        const auto writeInit        = static_cast<n2t::TranslationEngine::WriteInit>(isInputDirectory);

        n2t::PathList inputFilenames = findInputFiles(inputPath, isInputDirectory, outputFilename);

        /*
         * Translate input files
         */

        n2t::TranslationEngine engine(std::move(inputFilenames), std::move(outputFilename), writeInit);
        engine.translate();

        result = EXIT_SUCCESS;
    }
    catch (const po::required_option&)
    {
        std::cerr << "ERROR: No input path\n\n";
        displayHelp(programPath, visibleOptions);
    }
    catch (const po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n\n";
        displayHelp(programPath, visibleOptions);
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
    }

    return result;
}
