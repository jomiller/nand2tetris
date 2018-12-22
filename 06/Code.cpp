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

#include "Code.h"

#include "AsmUtil.h"

#include <map>

uint16_t n2t::Code::dest(const std::string& dest)
{
    uint16_t destCode = 0b000;
    if (!dest.empty())
    {
        if (dest == "M")
        {
            destCode = 0b001;
        }
        else if (dest == "D")
        {
            destCode = 0b010;
        }
        else if (dest == "MD")
        {
            destCode = 0b011;
        }
        else if (dest == "A")
        {
            destCode = 0b100;
        }
        else if (dest == "AM")
        {
            destCode = 0b101;
        }
        else if (dest == "AD")
        {
            destCode = 0b110;
        }
        else if (dest == "AMD")
        {
            destCode = 0b111;
        }
        else
        {
            AsmUtil::throwUncond("Invalid 'dest' mnemonic (" + dest + ")");
        }
    }
    return destCode;
}

uint16_t n2t::Code::comp(const std::string& comp)
{
    // clang-format off
    static const std::map<std::string_view, uint16_t> compCodes =
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
    };
    // clang-format on

    const auto iter = compCodes.find(comp);
    ASM_THROW_COND(iter != compCodes.end(), "Invalid 'comp' mnemonic (" + comp + ")");

    return iter->second;
}

uint16_t n2t::Code::jump(const std::string& jump)
{
    uint16_t jumpCode = 0b000;
    if (!jump.empty())
    {
        if (jump == "JGT")
        {
            jumpCode = 0b001;
        }
        else if (jump == "JEQ")
        {
            jumpCode = 0b010;
        }
        else if (jump == "JGE")
        {
            jumpCode = 0b011;
        }
        else if (jump == "JLT")
        {
            jumpCode = 0b100;
        }
        else if (jump == "JNE")
        {
            jumpCode = 0b101;
        }
        else if (jump == "JLE")
        {
            jumpCode = 0b110;
        }
        else if (jump == "JMP")
        {
            jumpCode = 0b111;
        }
        else
        {
            AsmUtil::throwUncond("Invalid 'jump' mnemonic (" + jump + ")");
        }
    }
    return jumpCode;
}
