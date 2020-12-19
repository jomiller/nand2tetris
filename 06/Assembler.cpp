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

#include <cxxopts.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

int main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    const std::filesystem::path programPath{*argv};
    cxxopts::Options            options{programPath.filename(), "Hack Assembler"};

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
            ("o,output-file", "Output binary file", cxxopts::value<std::filesystem::path>(outputFilename));

        options.add_options("Positional")
            ("input-file", "Input assembly file", cxxopts::value<std::vector<std::string>>());
        // clang-format on

        options.parse_positional("input-file");

        const auto optionsMap = options.parse(argc, argv);

        if (optionsMap.count("help") != 0)
        {
            std::cout << options.help() << '\n';
            return EXIT_SUCCESS;
        }

        /*
         * Get and validate input and output filenames
         */

        const auto inputFileCount = optionsMap.count("input-file");
        if (inputFileCount == 0)
        {
            throw cxxopts::option_required_exception{"input-file"};
        }
        if (inputFileCount != 1)
        {
            throw cxxopts::OptionParseException{"Option 'input-file' is specified more than once"};
        }

        std::filesystem::path inputFilename{optionsMap["input-file"].as<std::vector<std::string>>().front()};
        if (!std::filesystem::exists(inputFilename))
        {
            throw std::invalid_argument{"Input file (" + inputFilename.string() + ") does not exist"};
        }
        if (!std::filesystem::is_regular_file(inputFilename))
        {
            throw std::invalid_argument{"Input file (" + inputFilename.string() + ") is not a file"};
        }

        if (outputFilename.empty())
        {
            outputFilename = inputFilename;
            outputFilename.replace_extension(".hack");
        }

        /*
         * Assemble input file
         */

        n2t::AssemblyEngine engine{std::move(inputFilename), std::move(outputFilename)};
        engine.assemble();

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
