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

#include "TranslationEngine.h"
#include "VmTypes.h"

#include <cxxopts.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
[[nodiscard]] n2t::PathList findInputFiles(const std::filesystem::path& inputPath,
                                           bool                         isInputDirectory,
                                           std::filesystem::path&       outputFilename)
{
    n2t::PathList inputFilenames;

    if (isInputDirectory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(inputPath))
        {
            const auto& path = entry.path();
            if (std::filesystem::is_regular_file(path) && (path.extension() == ".vm"))
            {
                inputFilenames.push_back(path);
            }
        }

        if (inputFilenames.empty())
        {
            throw std::invalid_argument{
                fmt::format("Input directory ({}) does not contain VM files", inputPath.string())};
        }

        if (outputFilename.empty())
        {
            const auto inputPathAbsolute = std::filesystem::absolute(inputPath);
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
            throw std::invalid_argument{fmt::format("Input file ({}) is not a VM file", inputPath.string())};
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
        throw std::invalid_argument{
            fmt::format("Input path ({}) is neither a file nor a directory", inputPath.string())};
    }

    return inputFilenames;
}

}  // namespace

int main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    const std::filesystem::path programPath{*argv};
    cxxopts::Options            options{programPath.filename(), "VM Translator"};

    try
    {
        /*
         * Parse command line options
         */

        std::filesystem::path outputFilename;

        options.show_positional_help();

        // clang-format off
        options.add_options()
            ("help", "Display this help message")
            ("o,output-file", "Output assembly file", cxxopts::value<std::filesystem::path>(outputFilename));

        options.add_options("Positional")
            ("input-path", "Input VM file/directory", cxxopts::value<std::vector<std::string>>());
        // clang-format on

        options.parse_positional("input-path");

        const auto optionsMap = options.parse(argc, argv);

        if (optionsMap.count("help") != 0)
        {
            std::cout << options.help() << '\n';
            return EXIT_SUCCESS;
        }

        /*
         * Find and validate input and output filenames
         */

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

        const auto isInputDirectory = std::filesystem::is_directory(inputPath);
        const auto writeInit        = static_cast<n2t::TranslationEngine::WriteInit>(isInputDirectory);

        auto inputFilenames = findInputFiles(inputPath, isInputDirectory, outputFilename);

        /*
         * Translate input files
         */

        n2t::TranslationEngine engine{std::move(inputFilenames), std::move(outputFilename), writeInit};
        engine.translate();

        result = EXIT_SUCCESS;
    }
    catch (const cxxopts::OptionException& ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n\n";
        std::cout << options.help() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR: " << ex.what() << '\n';
    }

    return result;
}
