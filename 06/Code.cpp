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

#include "Code.h"

#include <Util.h>

#include <frozen/unordered_map.h>

uint16_t n2t::Code::dest(std::string_view dest)
{
    if (dest.empty())
    {
        return 0b000;
    }

    // clang-format off
    static constexpr auto destCodes = frozen::make_unordered_map<frozen::string, uint16_t>(
    {
        {"M",   uint16_t{0b001}},
        {"D",   uint16_t{0b010}},
        {"MD",  uint16_t{0b011}},
        {"A",   uint16_t{0b100}},
        {"AM",  uint16_t{0b101}},
        {"AD",  uint16_t{0b110}},
        {"AMD", uint16_t{0b111}}
    });
    // clang-format on

    const auto iter = destCodes.find(toFrozenString(dest));  // NOLINT(readability-qualified-auto)
    throwCond(iter != destCodes.end(), "Invalid 'dest' mnemonic ({})", dest);

    return iter->second;
}

uint16_t n2t::Code::comp(std::string_view comp)
{
    // clang-format off
    static constexpr auto compCodes = frozen::make_unordered_map<frozen::string, uint16_t>(
    {
        {"0",   uint16_t{0b0101010}},
        {"1",   uint16_t{0b0111111}},
        {"-1",  uint16_t{0b0111010}},
        {"D",   uint16_t{0b0001100}},
        {"A",   uint16_t{0b0110000}},
        {"M",   uint16_t{0b1110000}},
        {"!D",  uint16_t{0b0001101}},
        {"!A",  uint16_t{0b0110001}},
        {"!M",  uint16_t{0b1110001}},
        {"-D",  uint16_t{0b0001111}},
        {"-A",  uint16_t{0b0110011}},
        {"-M",  uint16_t{0b1110011}},
        {"D+1", uint16_t{0b0011111}},
        {"A+1", uint16_t{0b0110111}},
        {"M+1", uint16_t{0b1110111}},
        {"D-1", uint16_t{0b0001110}},
        {"A-1", uint16_t{0b0110010}},
        {"M-1", uint16_t{0b1110010}},
        {"D+A", uint16_t{0b0000010}},
        {"D+M", uint16_t{0b1000010}},
        {"D-A", uint16_t{0b0010011}},
        {"D-M", uint16_t{0b1010011}},
        {"A-D", uint16_t{0b0000111}},
        {"M-D", uint16_t{0b1000111}},
        {"D&A", uint16_t{0b0000000}},
        {"D&M", uint16_t{0b1000000}},
        {"D|A", uint16_t{0b0010101}},
        {"D|M", uint16_t{0b1010101}}
    });
    // clang-format on

    const auto iter = compCodes.find(toFrozenString(comp));  // NOLINT(readability-qualified-auto)
    throwCond(iter != compCodes.end(), "Invalid 'comp' mnemonic ({})", comp);

    return iter->second;
}

uint16_t n2t::Code::jump(std::string_view jump)
{
    if (jump.empty())
    {
        return 0b000;
    }

    static constexpr auto jumpCodes = frozen::make_unordered_map<frozen::string, uint16_t>({{"JGT", uint16_t{0b001}},
                                                                                            {"JEQ", uint16_t{0b010}},
                                                                                            {"JGE", uint16_t{0b011}},
                                                                                            {"JLT", uint16_t{0b100}},
                                                                                            {"JNE", uint16_t{0b101}},
                                                                                            {"JLE", uint16_t{0b110}},
                                                                                            {"JMP", uint16_t{0b111}}});

    const auto iter = jumpCodes.find(toFrozenString(jump));  // NOLINT(readability-qualified-auto)
    throwCond(iter != jumpCodes.end(), "Invalid 'jump' mnemonic ({})", jump);

    return iter->second;
}
