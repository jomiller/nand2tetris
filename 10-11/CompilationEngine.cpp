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

#include "CompilationEngine.h"

#include "JackAssert.h"
#include "JackUtil.h"

#include <fmt/format.h>

#include <limits>
#include <map>

n2t::CompilationEngine::CompilationEngine(const std::filesystem::path& inputFilename,
                                          std::filesystem::path        vmOutputFilename,
                                          const std::filesystem::path& xmlOutputFilename) :
    m_inputTokenizer{inputFilename},
    m_vmWriter{std::move(vmOutputFilename)},
    m_className{inputFilename.stem().string()}
{
    if (!xmlOutputFilename.empty())
    {
        m_xmlWriter = std::make_unique<XmlWriter>(xmlOutputFilename);
    }
    m_inputTokenizer.advance();
}

void n2t::CompilationEngine::compileClass()
{
    JackUtil::throwCond(!m_vmWriter.isClosed() && (!m_xmlWriter || !m_xmlWriter->isClosed()),
                        "Input file ({}) has already been compiled",
                        m_inputTokenizer.filename());

    try
    {
        XmlWriter::Element element{m_xmlWriter.get(), "class"};

        compileKeyword(Keyword::Class);
        const auto className = compileIdentifier("class");
        JackUtil::throwCond(
            className == m_className, "Class name ({}) does not match filename ()", className, m_className);

        compileSymbol('{');
        while ((m_inputTokenizer.tokenType() == TokenType::Keyword) && isClassVarDec(m_inputTokenizer.keyword()))
        {
            compileClassVarDec();
        }
        while ((m_inputTokenizer.tokenType() == TokenType::Keyword) && isSubroutine(m_inputTokenizer.keyword()))
        {
            compileSubroutine();
        }
        compileSymbol('}', /* optional = */ false, /* advance = */ false);

        JackUtil::throwCond(!m_inputTokenizer.hasMoreTokens(), "Expected end of file");
    }
    catch (const std::exception& ex)
    {
        JackUtil::throwUncond({m_inputTokenizer.filename(), m_inputTokenizer.lineNumber()}, ex.what());
    }

    validateSubroutineCalls();
    m_vmWriter.close();
    if (m_xmlWriter)
    {
        m_xmlWriter->close();
    }
}

bool n2t::CompilationEngine::isClassVarDec(Keyword keyword)
{
    return ((keyword == Keyword::Static) || (keyword == Keyword::Field));
}

bool n2t::CompilationEngine::isVarType(Keyword keyword, bool orVoid)
{
    return ((keyword == Keyword::Int) || (keyword == Keyword::Char) || (keyword == Keyword::Boolean) ||
            (orVoid && (keyword == Keyword::Void)));
}

bool n2t::CompilationEngine::isSubroutine(Keyword keyword)
{
    return ((keyword == Keyword::Constructor) || (keyword == Keyword::Function) || (keyword == Keyword::Method));
}

bool n2t::CompilationEngine::isStatement(Keyword keyword)
{
    return ((keyword == Keyword::Let) || (keyword == Keyword::Do) || (keyword == Keyword::If) ||
            (keyword == Keyword::While) || (keyword == Keyword::Return));
}

bool n2t::CompilationEngine::isBinaryOperator(char symbol)
{
    return ((symbol == '+') || (symbol == '-') || (symbol == '*') || (symbol == '/') || (symbol == '&') ||
            (symbol == '|') || (symbol == '<') || (symbol == '>') || (symbol == '='));
}

bool n2t::CompilationEngine::isUnaryOperator(char symbol)
{
    return ((symbol == '-') || (symbol == '~'));
}

bool n2t::CompilationEngine::isKeywordConstant(Keyword keyword)
{
    return ((keyword == Keyword::True) || (keyword == Keyword::False) || (keyword == Keyword::Null) ||
            (keyword == Keyword::This));
}

n2t::VariableKind n2t::CompilationEngine::getVariableKind(Keyword variableKind)
{
    // clang-format off
    switch (variableKind)
    {
        case Keyword::Static: return VariableKind::Static;
        case Keyword::Field:  return VariableKind::Field;
        case Keyword::Var:    return VariableKind::Local;
        default:              N2T_JACK_ASSERT(!"Invalid variable kind"); return VariableKind::None;
    }
    // clang-format on
}

n2t::SegmentType n2t::CompilationEngine::getSegmentType(VariableKind kind)
{
    // clang-format off
    switch (kind)
    {
        case VariableKind::Static:   return SegmentType::Static;
        case VariableKind::Field:    return SegmentType::This;
        case VariableKind::Argument: return SegmentType::Argument;
        case VariableKind::Local:    return SegmentType::Local;
        default:                     N2T_JACK_ASSERT(!"No memory segment associated with variable kind");
                                     return SegmentType(0);
    }
    // clang-format on
}

n2t::ArithmeticCommand n2t::CompilationEngine::getArithmeticCommand(char symbol, bool unary)
{
    // clang-format off
    static const std::map<char, ArithmeticCommand> commands =
    {
        {'+', ArithmeticCommand::Add},
        {'-', ArithmeticCommand::Sub},
        {'&', ArithmeticCommand::And},
        {'|', ArithmeticCommand::Or},
        {'~', ArithmeticCommand::Not},
        {'<', ArithmeticCommand::Lt},
        {'=', ArithmeticCommand::Eq},
        {'>', ArithmeticCommand::Gt}
    };
    // clang-format on

    const auto cmd = commands.find(symbol);
    N2T_JACK_ASSERT((cmd != commands.end()) && "Invalid arithmetic operator");
    if (unary && (cmd->second == ArithmeticCommand::Sub))
    {
        return ArithmeticCommand::Neg;
    }
    return cmd->second;
}

void n2t::CompilationEngine::compileClassVarDec()
{
    XmlWriter::Element element{m_xmlWriter.get(), "classVarDec"};

    compileVarDecImpl(m_inputTokenizer.keyword());
}

void n2t::CompilationEngine::compileSubroutine()
{
    XmlWriter::Element element{m_xmlWriter.get(), "subroutineDec"};

    m_symbolTable.startSubroutine();
    m_nextLabelId = 0;

    // compile subroutine signature
    m_currentSubroutine.type = m_inputTokenizer.keyword();
    compileKeyword(m_currentSubroutine.type);

    const auto returnType        = compileVarType(/* orVoid = */ true);
    const auto returnTypeKeyword = JackUtil::toKeyword(returnType);
    m_currentSubroutine.isVoid   = (returnTypeKeyword.second && (returnTypeKeyword.first == Keyword::Void));
    if (m_currentSubroutine.type == Keyword::Constructor)
    {
        JackUtil::throwCond(returnType == m_className,
                            "Constructor return type ({}) is not of the class type ({})",
                            returnType,
                            m_className);
    }
    else if (m_currentSubroutine.type == Keyword::Method)
    {
        // the first argument is a pointer to the object for which the method is invoked
        m_symbolTable.define("this", m_className, VariableKind::Argument);
    }

    m_currentSubroutine.name = compileIdentifier("subroutine");
    JackUtil::throwCond(m_definedSubroutines.find(m_currentSubroutine.name) == m_definedSubroutines.end(),
                        "Subroutine with name ({}) already defined",
                        m_currentSubroutine.name);

    m_currentSubroutine.numParameters = 0;
    compileSymbol('(');
    compileParameterList();
    compileSymbol(')');

    compileSubroutineBody();

    m_definedSubroutines.emplace(m_currentSubroutine.name, m_currentSubroutine);
}

void n2t::CompilationEngine::compileParameterList()
{
    XmlWriter::Element element{m_xmlWriter.get(), "parameterList"};

    if ((m_inputTokenizer.tokenType() != TokenType::Symbol) || (m_inputTokenizer.symbol() != ')'))
    {
        compileParameter();
        while (compileSymbol(',', /* optional = */ true))
        {
            compileParameter();
        }
    }
}

void n2t::CompilationEngine::compileVarDec()
{
    XmlWriter::Element element{m_xmlWriter.get(), "varDec"};

    compileVarDecImpl(m_inputTokenizer.keyword());
}

void n2t::CompilationEngine::compileStatements()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "statements"};

    while ((m_inputTokenizer.tokenType() == TokenType::Keyword) && isStatement(m_inputTokenizer.keyword()))
    {
        // clang-format off
        switch (m_inputTokenizer.keyword())
        {
            case Keyword::Let:    compileLet(); break;
            case Keyword::Do:     compileDo(); break;
            case Keyword::If:     compileIf(); break;
            case Keyword::While:  compileWhile(); break;
            case Keyword::Return: compileReturn(); break;
            default: break;
        }
        // clang-format on
    }
}

void n2t::CompilationEngine::compileLet()
{
    XmlWriter::Element element{m_xmlWriter.get(), "letStatement"};

    compileKeyword(Keyword::Let);
    const auto variableName = compileIdentifier("variable");
    bool       arrayEntry   = false;
    if ((m_inputTokenizer.tokenType() == TokenType::Symbol) && (m_inputTokenizer.symbol() == '['))
    {
        arrayEntry = true;
        compileArrayEntry(variableName);
    }
    compileSymbol('=');
    compileExpression();
    compileSymbol(';');

    if (arrayEntry)
    {
        // store the expression value to the 'temp' memory segment
        m_vmWriter.writePop(SegmentType::Temp, /* index = */ 0);

        // store a pointer to the destination array entry into the 'that' memory segment
        m_vmWriter.writePop(SegmentType::Pointer, /* index = */ 1);

        // store the expression value to the destination array entry
        m_vmWriter.writePush(SegmentType::Temp, /* index = */ 0);
        m_vmWriter.writePop(SegmentType::That, /* index = */ 0);
    }
    else
    {
        // store the expression value to the destination variable
        const VariableKind kind = getKindOf(variableName);
        JackUtil::throwCond(
            kind != VariableKind::None, "Identifier ({}) not defined in the current scope", variableName);

        m_vmWriter.writePop(getSegmentType(kind), m_symbolTable.indexOf(variableName));
    }
}

void n2t::CompilationEngine::compileDo()
{
    XmlWriter::Element element{m_xmlWriter.get(), "doStatement"};

    compileKeyword(Keyword::Do);
    compileSubroutineCall();
    compileSymbol(';');

    // discard the return value
    m_vmWriter.writePop(SegmentType::Temp, /* index = */ 0);
}

void n2t::CompilationEngine::compileIf()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "ifStatement"};

    compileKeyword(Keyword::If);
    compileSymbol('(');
    compileExpression();
    compileSymbol(')');

    m_vmWriter.writeArithmetic(ArithmeticCommand::Not);
    auto endLabel = fmt::format("IF{}", getNextLabelId());
    m_vmWriter.writeIf(endLabel);
    compileSymbol('{');
    compileStatements();
    compileSymbol('}');

    if (compileKeyword(Keyword::Else, /* optional = */ true))
    {
        const auto elseLabel = endLabel;
        endLabel             = fmt::format("IF{}", getNextLabelId());
        m_vmWriter.writeGoto(endLabel);
        m_vmWriter.writeLabel(elseLabel);
        compileSymbol('{');
        compileStatements();
        compileSymbol('}');
    }
    m_vmWriter.writeLabel(endLabel);
}

void n2t::CompilationEngine::compileWhile()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "whileStatement"};

    const auto beginLabel = fmt::format("WHILE{}", getNextLabelId());
    m_vmWriter.writeLabel(beginLabel);
    compileKeyword(Keyword::While);
    compileSymbol('(');
    compileExpression();
    compileSymbol(')');

    m_vmWriter.writeArithmetic(ArithmeticCommand::Not);
    const auto endLabel = fmt::format("WHILE{}", getNextLabelId());
    m_vmWriter.writeIf(endLabel);
    compileSymbol('{');
    compileStatements();
    compileSymbol('}');
    m_vmWriter.writeGoto(beginLabel);
    m_vmWriter.writeLabel(endLabel);
}

void n2t::CompilationEngine::compileReturn()
{
    XmlWriter::Element element{m_xmlWriter.get(), "returnStatement"};

    m_inReturnStatement = true;
    compileKeyword(Keyword::Return);
    if ((m_inputTokenizer.tokenType() != TokenType::Symbol) || (m_inputTokenizer.symbol() != ';'))
    {
        JackUtil::throwCond(!m_currentSubroutine.isVoid, "Void subroutine returns a value");

        compileExpression();
    }
    else
    {
        JackUtil::throwCond(m_currentSubroutine.type != Keyword::Constructor, "Constructor does not return 'this'");

        JackUtil::throwCond(m_currentSubroutine.isVoid, "Non-void subroutine does not return a value");

        // return constant 0 from void subroutine
        m_vmWriter.writePush(SegmentType::Constant, /* index = */ 0);
    }
    compileSymbol(';');
    m_inReturnStatement = false;
    m_vmWriter.writeReturn();
}

void n2t::CompilationEngine::compileExpression()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "expression"};

    compileTerm();
    while ((m_inputTokenizer.tokenType() == TokenType::Symbol) && isBinaryOperator(m_inputTokenizer.symbol()))
    {
        JackUtil::throwCond(!m_inReturnStatement || (m_currentSubroutine.type != Keyword::Constructor),
                            "Constructor does not return 'this'");

        const auto symbol = m_inputTokenizer.symbol();
        compileSymbol(symbol);
        compileTerm();
        if (symbol == '*')
        {
            m_vmWriter.writeCall("Math.multiply", /* numArguments = */ 2);
        }
        else if (symbol == '/')
        {
            m_vmWriter.writeCall("Math.divide", /* numArguments = */ 2);
        }
        else
        {
            m_vmWriter.writeArithmetic(getArithmeticCommand(symbol, /* unary = */ false));
        }
    }
}

void n2t::CompilationEngine::compileTerm()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "term"};

    bool       thisKeyword = false;
    const auto tokenType   = m_inputTokenizer.tokenType();
    if ((tokenType == TokenType::Keyword) && isKeywordConstant(m_inputTokenizer.keyword()))
    {
        const auto keyword = m_inputTokenizer.keyword();
        compileKeyword(keyword);
        switch (keyword)
        {
            case Keyword::True:
                m_vmWriter.writePush(SegmentType::Constant, /* index = */ 0);
                m_vmWriter.writeArithmetic(ArithmeticCommand::Not);
                break;

            case Keyword::False:
            case Keyword::Null:
                m_vmWriter.writePush(SegmentType::Constant, /* index = */ 0);
                break;

            case Keyword::This:
                JackUtil::throwCond(m_currentSubroutine.type != Keyword::Function,
                                    "'this' referenced from within a function");

                m_vmWriter.writePush(SegmentType::Pointer, /* index = */ 0);
                thisKeyword = true;
                break;

            default:
                break;
        }
    }
    else if (tokenType == TokenType::IntConst)
    {
        const auto intConst = compileIntegerConstant();
        m_vmWriter.writePush(SegmentType::Constant, intConst);
    }
    else if (tokenType == TokenType::StringConst)
    {
        const auto    stringConst          = compileStringConstant();
        const auto    stringConstLength    = stringConst.length();
        const int16_t maxStringConstLength = std::numeric_limits<int16_t>::max();
        JackUtil::throwCond(stringConstLength <= static_cast<std::string::size_type>(maxStringConstLength),
                            "Length of string constant ({}) exceeds the limit ({})",
                            stringConst,
                            maxStringConstLength);

        m_vmWriter.writePush(SegmentType::Constant, static_cast<int16_t>(stringConstLength));
        m_vmWriter.writeCall("String.new", /* numArguments = */ 1);
        for (auto c : stringConst)
        {
            m_vmWriter.writePush(SegmentType::Constant, c);
            m_vmWriter.writeCall("String.appendChar", /* numArguments = */ 2);
        }
    }
    else if ((tokenType == TokenType::Symbol) &&
             ((m_inputTokenizer.symbol() == '(') || isUnaryOperator(m_inputTokenizer.symbol())))
    {
        if (compileSymbol('(', /* optional = */ true))
        {
            compileExpression();
            compileSymbol(')');
            thisKeyword = true;
        }
        else  // unary arithmetic operator
        {
            const auto symbol = m_inputTokenizer.symbol();
            compileSymbol(symbol);
            compileTerm();
            m_vmWriter.writeArithmetic(getArithmeticCommand(symbol, /* unary = */ true));
        }
    }
    else if (tokenType == TokenType::Identifier)
    {
        const auto identifier = compileIdentifier();
        if ((m_inputTokenizer.tokenType() == TokenType::Symbol) &&
            ((m_inputTokenizer.symbol() == '[') || (m_inputTokenizer.symbol() == '(') ||
             (m_inputTokenizer.symbol() == '.')))
        {
            if (m_inputTokenizer.symbol() == '[')
            {
                compileArrayEntry(identifier);
                m_vmWriter.writePop(SegmentType::Pointer, /* index = */ 1);
                m_vmWriter.writePush(SegmentType::That, /* index = */ 0);
            }
            else
            {
                compileSubroutineCall(identifier);
            }
        }
        else
        {
            const auto kind = getKindOf(identifier);
            JackUtil::throwCond(
                kind != VariableKind::None, "Identifier ({}) not defined in the current scope", identifier);

            m_vmWriter.writePush(getSegmentType(kind), m_symbolTable.indexOf(identifier));
        }
    }
    else
    {
        JackUtil::throwUncond("Expected expression before {}", getTokenDescription());
    }

    JackUtil::throwCond(!m_inReturnStatement || (m_currentSubroutine.type != Keyword::Constructor) || thisKeyword,
                        "Constructor does not return 'this'");
}

void n2t::CompilationEngine::compileExpressionList()  // NOLINT(misc-no-recursion)
{
    XmlWriter::Element element{m_xmlWriter.get(), "expressionList"};

    if ((m_inputTokenizer.tokenType() != TokenType::Symbol) || (m_inputTokenizer.symbol() != ')'))
    {
        compileExpression();
        ++m_argumentCounts.top();
        while (compileSymbol(',', /* optional = */ true))
        {
            compileExpression();
            ++m_argumentCounts.top();
        }
    }
}

bool n2t::CompilationEngine::compileKeyword(Keyword expected, bool optional)
{
    if ((m_inputTokenizer.tokenType() != TokenType::Keyword) || (m_inputTokenizer.keyword() != expected))
    {
        if (optional)
        {
            return false;
        }
        JackUtil::throwUncond("Expected keyword ({}) before {}", JackUtil::toString(expected), getTokenDescription());
    }
    if (m_xmlWriter)
    {
        m_xmlWriter->writeKeyword(m_inputTokenizer.keyword());
    }
    m_inputTokenizer.advance();
    return true;
}

bool n2t::CompilationEngine::compileSymbol(char expected, bool optional, bool advance)
{
    if ((m_inputTokenizer.tokenType() != TokenType::Symbol) || (m_inputTokenizer.symbol() != expected))
    {
        if (optional)
        {
            return false;
        }
        JackUtil::throwUncond("Expected symbol ({}) before {}", expected, getTokenDescription());
    }
    if (m_xmlWriter)
    {
        m_xmlWriter->writeSymbol(m_inputTokenizer.symbol());
    }
    if (advance)
    {
        m_inputTokenizer.advance();
    }
    return true;
}

std::string n2t::CompilationEngine::compileIdentifier(std::string_view type)
{
    JackUtil::throwCond(m_inputTokenizer.tokenType() == TokenType::Identifier,
                        "Expected {} name before {}",
                        type,
                        getTokenDescription());

    auto identifier = m_inputTokenizer.identifier();
    if (m_xmlWriter)
    {
        m_xmlWriter->writeIdentifier(identifier);
    }
    m_inputTokenizer.advance();
    return identifier;
}

int16_t n2t::CompilationEngine::compileIntegerConstant()
{
    JackUtil::throwCond(m_inputTokenizer.tokenType() == TokenType::IntConst,
                        "Expected integer constant before {}",
                        getTokenDescription());

    const auto intConst = m_inputTokenizer.intVal();
    if (m_xmlWriter)
    {
        m_xmlWriter->writeIntegerConstant(intConst);
    }
    m_inputTokenizer.advance();
    return intConst;
}

std::string n2t::CompilationEngine::compileStringConstant()
{
    JackUtil::throwCond(m_inputTokenizer.tokenType() == TokenType::StringConst,
                        "Expected string constant before {}",
                        getTokenDescription());

    auto stringConst = m_inputTokenizer.stringVal();
    if (m_xmlWriter)
    {
        m_xmlWriter->writeStringConstant(stringConst);
    }
    m_inputTokenizer.advance();
    return stringConst;
}

void n2t::CompilationEngine::compileVarDecImpl(Keyword variableKind)
{
    compileKeyword(variableKind);
    const auto kind = getVariableKind(variableKind);
    const auto type = compileVarType();
    const auto name = compileIdentifier("variable");
    m_symbolTable.define(name, type, kind);
    while (compileSymbol(',', /* optional = */ true))
    {
        const auto nextName = compileIdentifier("variable");
        m_symbolTable.define(nextName, type, kind);
    }
    compileSymbol(';');
}

std::string n2t::CompilationEngine::compileVarType(bool orVoid)
{
    const auto tokenType = m_inputTokenizer.tokenType();
    if (tokenType == TokenType::Identifier)
    {
        return compileIdentifier();
    }
    JackUtil::throwCond((tokenType == TokenType::Keyword) && isVarType(m_inputTokenizer.keyword(), orVoid),
                        "Expected class name or variable type before {}",
                        getTokenDescription());

    const auto keyword = m_inputTokenizer.keyword();
    compileKeyword(keyword);
    return std::string{JackUtil::toString(keyword)};
}

void n2t::CompilationEngine::compileParameter()
{
    const auto type = compileVarType();
    const auto name = compileIdentifier("variable");
    m_symbolTable.define(name, type, VariableKind::Argument);
    ++m_currentSubroutine.numParameters;
}

void n2t::CompilationEngine::compileSubroutineBody()
{
    XmlWriter::Element element{m_xmlWriter.get(), "subroutineBody"};

    compileSymbol('{');
    while ((m_inputTokenizer.tokenType() == TokenType::Keyword) && (m_inputTokenizer.keyword() == Keyword::Var))
    {
        compileVarDec();
    }

    m_vmWriter.writeFunction(fmt::format("{}.{}", m_className, m_currentSubroutine.name),
                             m_symbolTable.varCount(VariableKind::Local));

    if (m_currentSubroutine.type == Keyword::Constructor)
    {
        // allocate at least one word so that objects of field-less classes will still have valid addresses
        const int16_t numFields = std::max(m_symbolTable.varCount(VariableKind::Field), int16_t{1});
        m_vmWriter.writePush(SegmentType::Constant, numFields);
        m_vmWriter.writeCall("Memory.alloc", /* numArguments = */ 1);

        // store a pointer to the new object into the 'this' memory segment
        m_vmWriter.writePop(SegmentType::Pointer, /* index = */ 0);
    }
    else if (m_currentSubroutine.type == Keyword::Method)
    {
        // store a pointer to the object for which the method is invoked into the 'this' memory segment
        m_vmWriter.writePush(SegmentType::Argument, /* index = */ 0);
        m_vmWriter.writePop(SegmentType::Pointer, /* index = */ 0);
    }

    compileStatements();
    compileSymbol('}');
}

void n2t::CompilationEngine::compileArrayEntry(const std::string& variableName)  // NOLINT(misc-no-recursion)
{
    JackUtil::throwCond(m_symbolTable.typeOf(variableName) == "Array",
                        "Array entry accessed in variable ({}) that is not of type Array",
                        variableName);

    // the expression value is the index of the array entry
    compileSymbol('[');
    compileExpression();
    compileSymbol(']');

    // add the index of the array entry to the array base address
    const auto kind = getKindOf(variableName);
    JackUtil::throwCond(kind != VariableKind::None, "Identifier ({}) not defined in the current scope", variableName);

    m_vmWriter.writePush(getSegmentType(kind), m_symbolTable.indexOf(variableName));
    m_vmWriter.writeArithmetic(ArithmeticCommand::Add);
}

void n2t::CompilationEngine::compileSubroutineCall(const std::string& identifier)  // NOLINT(misc-no-recursion)
{
    SubroutineCallInfo subroutineCall;
    auto               subroutineName = identifier;
    if (subroutineName.empty())
    {
        subroutineName = compileIdentifier("subroutine, class or object");
    }
    else
    {
        subroutineCall.expression = true;
    }

    std::string classObjectName;
    if (compileSymbol('.', /* optional = */ true))
    {
        // either a method is invoked for a non-current object or a constructor/function is invoked
        classObjectName = subroutineName;
        subroutineName  = compileIdentifier("subroutine");
    }

    subroutineCall.name = subroutineName;
    bool thisArgument   = false;
    if (classObjectName.empty())
    {
        // a method is invoked for the current object
        subroutineCall.type = Keyword::Method;
        JackUtil::throwCond(m_currentSubroutine.type != Keyword::Function,
                            "Subroutine ({}) called as a method from within a function",
                            subroutineName);

        // set the first argument to be a pointer to the current object
        thisArgument = true;
        m_vmWriter.writePush(SegmentType::Pointer, /* index = */ 0);
        classObjectName = m_className;
    }
    else
    {
        const auto objectKind = getKindOf(classObjectName);
        if (objectKind != VariableKind::None)
        {
            // a method is invoked for a non-current object
            subroutineCall.type = Keyword::Method;

            // set the first argument to be a pointer to the object
            thisArgument = true;
            m_vmWriter.writePush(getSegmentType(objectKind), m_symbolTable.indexOf(classObjectName));
            classObjectName = m_symbolTable.typeOf(classObjectName);
        }
    }

    m_argumentCounts.push(0);
    compileSymbol('(');
    compileExpressionList();
    compileSymbol(')');
    const auto currentArgumentCount = m_argumentCounts.top();
    subroutineCall.numArguments     = currentArgumentCount;

    // the number of arguments is determined from the number of expressions in the expression list
    m_vmWriter.writeCall(fmt::format("{}.{}", classObjectName, subroutineName),
                         currentArgumentCount + static_cast<int16_t>(thisArgument));

    if (classObjectName == m_className)
    {
        m_calledSubroutines.insert(subroutineCall);
    }
    m_argumentCounts.pop();
}

void n2t::CompilationEngine::validateSubroutineCalls() const
{
    const auto mainSub = m_definedSubroutines.find("main");
    JackUtil::throwCond((m_className != "Main") ||
                            ((mainSub != m_definedSubroutines.end()) && (mainSub->second.type == Keyword::Function)),
                        "Class does not contain a function named 'main'");

    for (const SubroutineCallInfo& call : m_calledSubroutines)
    {
        const auto sub = m_definedSubroutines.find(call.name);
        JackUtil::throwCond(sub != m_definedSubroutines.end(), "Undefined reference to subroutine ({})", call.name);

        JackUtil::throwCond((sub->second.type != Keyword::Constructor) || (call.type != Keyword::Method),
                            "Constructor ({}) called as a method",
                            sub->first);

        JackUtil::throwCond((sub->second.type != Keyword::Function) || (call.type != Keyword::Method),
                            "Function ({}) called as a method",
                            sub->first);

        JackUtil::throwCond((sub->second.type != Keyword::Method) || (call.type != Keyword::Function),
                            "Method ({}) called as a constructor/function",
                            sub->first);

        JackUtil::throwCond(sub->second.numParameters == call.numArguments,
                            "Subroutine ({}) declared to accept {} parameter(s) but called with {} argument(s)",
                            sub->first,
                            sub->second.numParameters,
                            call.numArguments);

        JackUtil::throwCond(
            !sub->second.isVoid || !call.expression, "Void subroutine ({}) used in an expression", sub->first);
    }
}

n2t::VariableKind n2t::CompilationEngine::getKindOf(const std::string& variableName) const
{
    const auto kind = m_symbolTable.kindOf(variableName);
    JackUtil::throwCond((kind != VariableKind::Field) || (m_currentSubroutine.type != Keyword::Function),
                        "Field variable ({}) referenced from within a function",
                        variableName);

    return kind;
}

unsigned int n2t::CompilationEngine::getNextLabelId()
{
    return m_nextLabelId++;
}

std::string n2t::CompilationEngine::getTokenDescription() const
{
    const auto  tokenType = m_inputTokenizer.tokenType();
    std::string token;

    // clang-format off
    switch (tokenType)
    {
        case TokenType::Keyword:     token = JackUtil::toString(m_inputTokenizer.keyword()); break;
        case TokenType::Symbol:      token = m_inputTokenizer.symbol(); break;
        case TokenType::Identifier:  token = m_inputTokenizer.identifier(); break;
        case TokenType::IntConst:    token = std::to_string(m_inputTokenizer.intVal()); break;
        case TokenType::StringConst: token = m_inputTokenizer.stringVal(); break;
        default:                     N2T_JACK_ASSERT(!"Invalid token type"); break;
    }
    // clang-format on

    return fmt::format("{} ({})", JackUtil::toString(tokenType), token);
}
