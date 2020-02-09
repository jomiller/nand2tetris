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

#include "AssemblyEngine.h"

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace
{
namespace po = boost::program_options;

void displayHelp(const std::filesystem::path& programPath, const po::options_description& options)
{
    std::cout << "Usage: " << programPath.filename().string() << " <filename>.asm\n\n";
    std::cout << options << '\n';
}

po::variables_map parseOptions(int argc, const char* const* argv, const po::options_description& visibleOptions)
{
    po::options_description hiddenOptions("Hidden Options");
    // clang-format off
    hiddenOptions.add_options()
        ("input-file", po::value<std::string>()->required(), "Input file");
    // clang-format on

    po::options_description cmdlineOptions;
    cmdlineOptions.add(visibleOptions).add(hiddenOptions);

    po::positional_options_description positionalOptions;
    positionalOptions.add("input-file", /* max_count = */ 1);

    po::variables_map optionsMap;

    po::store(po::command_line_parser(argc, argv).options(cmdlineOptions).positional(positionalOptions).run(),
              optionsMap);

    return optionsMap;
}
}  // namespace

int main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    const std::filesystem::path programPath(*argv);
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
         * Get and validate input and output filenames
         */

        std::filesystem::path inputFilename(optionsMap["input-file"].as<std::string>());
        if (!std::filesystem::exists(inputFilename))
        {
            throw std::invalid_argument("Input file (" + inputFilename.string() + ") does not exist");
        }
        if (!std::filesystem::is_regular_file(inputFilename))
        {
            throw std::invalid_argument("Input file (" + inputFilename.string() + ") is not a file");
        }

        if (outputFilename.empty())
        {
            outputFilename = inputFilename;
            outputFilename.replace_extension(".hack");
        }

        /*
         * Assemble input file
         */

        n2t::AssemblyEngine engine(std::move(inputFilename), std::move(outputFilename));
        engine.assemble();

        result = EXIT_SUCCESS;
    }
    catch (const po::required_option&)
    {
        std::cerr << "ERROR: No input file\n\n";
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
