#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

// 前向声明
class CodeGenContext;
class NBlock;
class NStatement;
class NIfStatement;
class NExpression;
class NVariableDeclaration;

// 类型定义
typedef std::vector<NBlock*> BlockList;
typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

// 基类 Node
class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
    virtual void print(int indent = 0) const {
        for(int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "$";
    }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NBlock : public NExpression {
public:
    StatementList statements;
    BlockList blocks;
    NBlock() { }
    NBlock(NStatement& statement) { statements.push_back(&statement); }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NReturnStatement : public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
        type(type), id(id) { assignmentExpr = NULL; }
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NConstDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr;
    VariableList arguments;  // 添加了 arguments 成员
    NConstDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) { }
    NConstDeclaration(const NIdentifier& type, NIdentifier& id,
                      NExpression *assignmentExpr, const VariableList& arguments) :
        type(type), id(id), assignmentExpr(assignmentExpr), arguments(arguments) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, 
            const VariableList& arguments, NBlock& block) :
        type(type), id(id), arguments(arguments), block(block) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NIfStatement : public NStatement {
public:
    NExpression& condition;
    NBlock& trueBlock;
    NBlock* falseBlock; // 使用指针以处理可选的 else 块
    NIfStatement(NExpression& condition, NBlock& trueBlock, NBlock& falseBlock) :
        condition(condition), trueBlock(trueBlock), falseBlock(&falseBlock) { };
    NIfStatement(NExpression& condition, NBlock& trueBlock) :
        condition(condition), trueBlock(trueBlock), falseBlock(nullptr) { };
    // virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};

class NWhileStatement : public NStatement {
public:
    NExpression& condition;
    NBlock& block;
    NWhileStatement(NExpression& condition, NBlock& block) :
        condition(condition), block(block) { };
    // virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
};