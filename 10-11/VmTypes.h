#ifndef VM_TYPES_H
#define VM_TYPES_H

namespace n2t
{
enum class ArithmeticCommand
{
    Add,
    Sub,
    Neg,
    And,
    Or,
    Not,
    Lt,
    Eq,
    Gt
};

enum class SegmentType
{
    Constant,
    Static,
    Pointer,
    Temp,
    Argument,
    Local,
    This,
    That
};
}  // namespace n2t

#endif
