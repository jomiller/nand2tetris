#ifndef VM_WRITER_H
#define VM_WRITER_H

#include "VmTypes.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace n2t
{
class VmWriter
{
public:
    // Creates a new file and prepares it for writing.
    explicit VmWriter(std::filesystem::path filename);

    VmWriter(const VmWriter&) = delete;
    VmWriter(VmWriter&&)      = delete;

    VmWriter& operator=(const VmWriter&) = delete;
    VmWriter& operator=(VmWriter&&) = delete;

    ~VmWriter() noexcept;

    // Writes a VM push command.
    void writePush(SegmentType segment, int16_t index);

    // Writes a VM pop command.
    void writePop(SegmentType segment, int16_t index);

    // Writes a VM arithmetic command.
    void writeArithmetic(ArithmeticCommand command);

    // Writes a VM label command.
    void writeLabel(const std::string& label);

    // Writes a VM goto command.
    void writeGoto(const std::string& label);

    // Writes a VM if-goto command.
    void writeIf(const std::string& label);

    // Writes a VM function command.
    void writeFunction(const std::string& functionName, unsigned int numLocals);

    // Writes a VM return command.
    void writeReturn();

    // Writes a VM call command.
    void writeCall(const std::string& functionName, unsigned int numArguments);

    // Closes the output file.
    void close();

    // Returns whether the output file has been closed.
    bool isClosed() const
    {
        return m_closed;
    }

private:
    static const std::string& toString(SegmentType segment);

    std::filesystem::path m_filename;
    std::ofstream         m_file;
    bool                  m_closed = false;
};
}  // namespace n2t

#endif
