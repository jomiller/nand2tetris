#ifndef CODE_H
#define CODE_H

#include <cstdint>
#include <string>

namespace n2t
{
class Code
{
public:
    Code() = delete;

    // Returns the binary code of the 'dest' mnemonic.
    static uint16_t dest(const std::string& dest);

    // Returns the binary code of the 'comp' mnemonic.
    static uint16_t comp(const std::string& comp);

    // Returns the binary code of the 'jump' mnemonic.
    static uint16_t jump(const std::string& jump);
};
}  // namespace n2t

#endif
