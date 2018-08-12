#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VmWriter.h"
#include "XmlWriter.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
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
        Keyword      type   = Keyword::Constructor;
        bool         isVoid = false;
        std::string  name;
        unsigned int numParameters = 0;

        SubroutineInfo() = default;

        SubroutineInfo(Keyword t, bool v, std::string n, unsigned int p) :
            type(t),
            isVoid(v),
            name(std::move(n)),
            numParameters(p)
        {
        }
    };

    struct SubroutineCallInfo
    {
        Keyword      type = Keyword::Function;
        std::string  name;
        unsigned int numArguments = 0;
        bool         expression   = false;

        SubroutineCallInfo() = default;

        SubroutineCallInfo(Keyword t, std::string n, unsigned int a, bool e) :
            type(t),
            name(std::move(n)),
            numArguments(a),
            expression(e)
        {
        }

        bool operator<(const SubroutineCallInfo& rhs) const
        {
            if (type != rhs.type)
            {
                return (type < rhs.type);
            }
            if (name != rhs.name)
            {
                return (name < rhs.name);
            }
            if (numArguments != rhs.numArguments)
            {
                return (numArguments < rhs.numArguments);
            }
            return (!expression && rhs.expression);
        }
    };

    static bool              isClassVarDec(Keyword keyword);
    static bool              isVarType(Keyword keyword, bool orVoid);
    static bool              isSubroutine(Keyword keyword);
    static bool              isStatement(Keyword keyword);
    static bool              isBinaryOperator(char symbol);
    static bool              isUnaryOperator(char symbol);
    static bool              isKeywordConstant(Keyword keyword);
    static VariableKind      getVariableKind(Keyword variableKind);
    static SegmentType       getSegmentType(VariableKind kind);
    static ArithmeticCommand getArithmeticCommand(char symbol, bool unary);

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

    bool         compileKeyword(Keyword expected, bool optional = false);
    bool         compileSymbol(char expected, bool optional = false, bool advance = true);
    std::string  compileIdentifier(const std::string& type = {});
    int16_t      compileIntegerConstant();
    std::string  compileStringConstant();
    void         compileVarDecImpl(Keyword variableKind);
    std::string  compileVarType(bool orVoid = false);
    void         compileParameter();
    void         compileSubroutineBody();
    void         compileArrayEntry(const std::string& variableName);
    void         compileSubroutineCall(const std::string& identifier = {});
    void         validateSubroutineCalls() const;
    VariableKind getKindOf(const std::string& variableName) const;
    unsigned int getNextLabelId();
    std::string  getTokenDescription() const;

    JackTokenizer                         m_inputTokenizer;
    SymbolTable                           m_symbolTable;
    VmWriter                              m_vmWriter;
    std::unique_ptr<XmlWriter>            m_xmlWriter;
    std::string                           m_className;
    SubroutineInfo                        m_currentSubroutine;
    std::map<std::string, SubroutineInfo> m_definedSubroutines;
    std::set<SubroutineCallInfo>          m_calledSubroutines;
    std::stack<unsigned int>              m_argumentCounts;
    bool                                  m_inReturnStatement = false;
    unsigned int                          m_nextLabelId       = 0;
};
}  // namespace n2t

#endif
