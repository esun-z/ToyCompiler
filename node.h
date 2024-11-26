#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

// 前向声明
class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

// 类型定义
typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

// 基类 Node
class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
	virtual void print(int indent = 0) const { 
		// 默认实现，可以为空或打印通用信息
		for(int i = 0; i < indent; ++i) std::cout << " ";
		std::cout << "$"; 
	}
};

// 表达式基类
class NExpression : public Node {
};

// 语句基类
class NStatement : public Node {
};

// 整数节点
class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 双精度浮点数节点
class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 标识符节点
class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 方法调用节点
class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 二元运算符节点
class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 赋值节点
class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 块节点
class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 表达式语句节点
class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 返回语句节点
class NReturnStatement : public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) : 
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 变量声明节点
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
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 外部声明节点
class NExternDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    NExternDeclaration(const NIdentifier& type, const NIdentifier& id,
            const VariableList& arguments) :
        type(type), id(id), arguments(arguments) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

// 函数声明节点
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
    virtual void print(int indent = 0) const override; // 声明 print 方法
};

#endif // NODE_H
