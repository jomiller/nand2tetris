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

#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VmWriter.h"
#include "XmlWriter.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace n2t
{
class CompilationEngine
{
public:
    // Creates a new compilation engine with the given input and output.
    CompilationEngine(const std::filesystem::path& inputFilename,
                      std::filesystem::path        vmOutputFilename,
                      const std::filesystem::path& xmlOutputFilename);

    // Compiles a complete class.
    void compileClass();

private:
    struct SubroutineInfo
    {
        SubroutineInfo() = default;

        SubroutineInfo(Keyword t, bool v, std::string n, int16_t p) :
            type{t},
            isVoid{v},
            name{std::move(n)},
            numParameters{p}
        {
        }

        Keyword     type   = Keyword::Constructor;
        bool        isVoid = false;
        std::string name;
        int16_t     numParameters = 0;
    };

    struct SubroutineCallInfo
    {
        SubroutineCallInfo() = default;

        SubroutineCallInfo(Keyword t, std::string n, int16_t a, bool e) :
            type{t},
            name{std::move(n)},
            numArguments{a},
            expression{e}
        {
        }

        [[nodiscard]] bool operator<(const SubroutineCallInfo& rhs) const
        {
            if (type != rhs.type)
            {
                return (type < rhs.type);
            }
            if (name != rhs.name)
            {
                return (name < rhs.name);  // NOLINT(modernize-use-nullptr)
            }
            if (numArguments != rhs.numArguments)
            {
                return (numArguments < rhs.numArguments);
            }
            return (!expression && rhs.expression);
        }

        Keyword     type = Keyword::Function;
        std::string name;
        int16_t     numArguments = 0;
        bool        expression   = false;
    };

    [[nodiscard]] static bool              isClassVarDec(Keyword keyword);
    [[nodiscard]] static bool              isVarType(Keyword keyword, bool orVoid);
    [[nodiscard]] static bool              isSubroutine(Keyword keyword);
    [[nodiscard]] static bool              isStatement(Keyword keyword);
    [[nodiscard]] static bool              isBinaryOperator(char symbol);
    [[nodiscard]] static bool              isUnaryOperator(char symbol);
    [[nodiscard]] static bool              isKeywordConstant(Keyword keyword);
    [[nodiscard]] static VariableKind      getVariableKind(Keyword variableKind);
    [[nodiscard]] static SegmentType       getSegmentType(VariableKind kind);
    [[nodiscard]] static ArithmeticCommand getArithmeticCommand(char symbol, bool unary);

    // Compiles a static declaration or a field declaration.
    void compileClassVarDec();

    // Compiles a complete constructor, function, or method.
    void compileSubroutine();

    // Compiles a (possibly empty) parameter list, not including the enclosing "()".
    void compileParameterList();

    // Compiles a var declaration.
    void compileVarDec();

    // Compiles a sequence of statements, not including the enclosing "{}".
    void compileStatements();

    // Compiles a let statement.
    void compileLet();

    // Compiles a do statement.
    void compileDo();

    // Compiles an if statement, possibly with a trailing else clause.
    void compileIf();

    // Compiles a while statement.
    void compileWhile();

    // Compiles a return statement.
    void compileReturn();

    // Compiles an expression.
    void compileExpression();

    // Compiles a term.
    void compileTerm();

    // Compiles a (possibly empty) comma-separated list of expressions.
    void compileExpressionList();

    bool                       compileKeyword(Keyword expected, bool optional = false);
    bool                       compileSymbol(char expected, bool optional = false, bool advance = true);
    [[nodiscard]] std::string  compileIdentifier(std::string_view type = {});
    [[nodiscard]] int16_t      compileIntegerConstant();
    [[nodiscard]] std::string  compileStringConstant();
    void                       compileVarDecImpl(Keyword variableKind);
    [[nodiscard]] std::string  compileVarType(bool orVoid = false);
    void                       compileParameter();
    void                       compileSubroutineBody();
    void                       compileArrayEntry(const std::string& variableName);
    void                       compileSubroutineCall(const std::string& identifier = {});
    void                       validateSubroutineCalls() const;
    [[nodiscard]] VariableKind getKindOf(const std::string& variableName) const;
    [[nodiscard]] unsigned int getNextLabelId();
    [[nodiscard]] std::string  getTokenDescription() const;

    JackTokenizer                                   m_inputTokenizer;
    SymbolTable                                     m_symbolTable;
    VmWriter                                        m_vmWriter;
    std::unique_ptr<XmlWriter>                      m_xmlWriter;
    std::string                                     m_className;
    SubroutineInfo                                  m_currentSubroutine;
    std::unordered_map<std::string, SubroutineInfo> m_definedSubroutines;
    std::set<SubroutineCallInfo>                    m_calledSubroutines;
    std::stack<int16_t>                             m_argumentCounts;
    bool                                            m_inReturnStatement = false;
    unsigned int                                    m_nextLabelId       = 0;
};
}  // namespace n2t

#endif
