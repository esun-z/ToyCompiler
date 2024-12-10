#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

// 前向声明
class CodeGenContext;
class NCompUnit;
class NStmt;
class NExpr;
class NDecl;
class NVarDecl;
class NFuncDecl;
class NBlock;
class NIdent;

// 类型定义
typedef std::vector<NStmt*> StmtList;
typedef std::vector<NExpr*> ExprList;
typedef std::vector<NVarDecl*> VariableList;
typedef std::vector<NDecl*> DeclList;

// enum class OperatorEnum {
//     PLUS,
//     MINUS,
//     MUL,
//     DIV,
//     MOD,
//     NEG,
//     CEQ,
//     CNE,
//     CLT,
//     CLE,
//     CGT,
//     CGE,
//     AND,
//     OR,
//     NOT,
//     UNKNOWN
// };

// 基类 Node
class Node {
public:
    Node() { }
    virtual ~Node() {}
    //virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
    virtual void print(int indent = 0) const;
    // 新增纯虚函数用于生成DOT
    virtual int generateDot(std::ostream& out, int& currentId) const;
};

class NCompUnit : public Node {
public:
    DeclList decls;
    NCompUnit() { }
    virtual ~NCompUnit() {
        for(auto decl : decls) {
            delete decl;
        }
        decls.clear();
    }
    NCompUnit(NDecl& decl) { decls.push_back(&decl); }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NExpr : public Node {
};

class NStmt : public Node {
};

class NDecl : public NStmt {
};

class NVarDecl : public NDecl {
public:
    bool isConst;
    NIdent& id;
    NExpr *assignmentExpr;
    NVarDecl(bool isConst, NIdent& id) :
        isConst(isConst), id(id) { assignmentExpr = NULL; }
    NVarDecl(bool isConst, NIdent& id, NExpr *assignmentExpr) :
        isConst(isConst), id(id), assignmentExpr(assignmentExpr) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NFuncDecl : public NDecl {
public:
    const NIdent& id;
    VariableList arguments;
    NBlock& block;
    NFuncDecl(const NIdent& id, 
            const VariableList& arguments, NBlock& block) :
        id(id), arguments(arguments), block(block) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NInteger : public NExpr {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NFloat : public NExpr {
public:
    double value;
    NFloat(double value) : value(value) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NIdent : public NExpr {
public:
    // enum class IdentType {
    //     INT,
    //     FLOAT,
    //     VOID,
    //     UNKNOWN
    // };
    std::string name;
    int type;
    NIdent(const std::string& name, int type) : name(name), type(type) { }
    NIdent(const std::string& name) : name(name), type(-1) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NMethodCall : public NExpr {
public:
    const NIdent& id;
    ExprList arguments;
    NMethodCall(const NIdent& id, ExprList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdent& id) : id(id) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NBinaryExpr : public NExpr {
public:
    int op;
    NExpr& lhs;
    NExpr& rhs;
    NBinaryExpr(NExpr& lhs, int op, NExpr& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NLogicalBinaryExpr : public NExpr {
public:

    int op;
    NExpr& lhs;
    NExpr& rhs;

    NLogicalBinaryExpr(NExpr& lhs, int op, NExpr& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }

    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NUnaryExpr : public NExpr {
public:

    int op;
    NExpr& expr;

    NUnaryExpr(int op, NExpr& expr) :
        op(op), expr(expr) { }

    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NLogicalUnaryExpr : public NExpr {
public:

    int op;
    NExpr &expr;

    NLogicalUnaryExpr(int op, NExpr &expr) : op(op), expr(expr) {}

    //virtual llvm::Value *codeGen(CodeGenContext &context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NAssignment : public NExpr {
public:
    NIdent& lhs;
    NExpr& rhs;
    NAssignment(NIdent& lhs, NExpr& rhs) : 
        lhs(lhs), rhs(rhs) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NBlock : public NStmt {
public:
    StmtList statements;
    NBlock() { }
    NBlock(NStmt& statement) { statements.push_back(&statement); }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NExprStmt : public NStmt {
public:
    NExpr& expression;
    NExprStmt(NExpr& expression) : 
        expression(expression) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NReturnStmt : public NStmt {
public:
    NExpr& expression;
    NReturnStmt(NExpr& expression) : 
        expression(expression) { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
     virtual int generateDot(std::ostream& out, int& currentId) const override;
};

// class NVariableDeclaration : public NStmt {
// public:
//     const NIdent& type;
//     NIdent& id;
//     NExpr *assignmentExpr;
//     NVariableDeclaration(const NIdent& type, NIdent& id) :
//         type(type), id(id) { assignmentExpr = NULL; }
//     NVariableDeclaration(const NIdent& type, NIdent& id, NExpr *assignmentExpr) :
//         type(type), id(id), assignmentExpr(assignmentExpr) { }
//     //virtual llvm::Value* codeGen(CodeGenContext& context);
//     virtual void print(int indent = 0) const override;
// };

// class NConstDeclaration : public NStatement {
// public:
//     const NIdentifier& type;
//     NIdentifier& id;
//     NExpression *assignmentExpr;
//     VariableList arguments;  // 添加了 arguments 成员
//     NConstDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
//         type(type), id(id), assignmentExpr(assignmentExpr) { }
//     NConstDeclaration(const NIdentifier& type, NIdentifier& id,
//                       NExpression *assignmentExpr, const VariableList& arguments) :
//         type(type), id(id), assignmentExpr(assignmentExpr), arguments(arguments) { }
//     //virtual llvm::Value* codeGen(CodeGenContext& context);
//     virtual void print(int indent = 0) const override;
// };

// class NFunctionDeclaration : public NStmt {
// public:
//     const NIdent& type;
//     const NIdent& id;
//     VariableList arguments;
//     NBlock& block;
//     NFunctionDeclaration(const NIdent& type, const NIdent& id, 
//             const VariableList& arguments, NBlock& block) :
//         type(type), id(id), arguments(arguments), block(block) { }
//     //virtual llvm::Value* codeGen(CodeGenContext& context);
//     virtual void print(int indent = 0) const override;
// };

class NIfStmt : public NStmt {
public:
    NExpr& condition;
    NBlock& trueBlock;
    NBlock* falseBlock; // 使用指针以处理可选的 else 块
    NIfStmt(NExpr& condition, NBlock& trueBlock, NBlock& falseBlock) :
        condition(condition), trueBlock(trueBlock), falseBlock(&falseBlock) { };
    NIfStmt(NExpr& condition, NBlock& trueBlock) :
        condition(condition), trueBlock(trueBlock), falseBlock(nullptr) { };
    // //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
    virtual int generateDot(std::ostream& out, int& currentId) const override;
};

class NWhileStmt : public NStmt {
public:
    NExpr& condition;
    NBlock& block;
    NWhileStmt(NExpr& condition, NBlock& block) :
        condition(condition), block(block) { };
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
    virtual int generateDot(std::ostream& out, int& currentId) const override;
    
};

class NBreakStmt : public NStmt {
public:
    NBreakStmt() { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
    virtual int generateDot(std::ostream& out, int& currentId) const override;
};
class NContinueStmt : public NStmt {
public:
    NContinueStmt() { }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void print(int indent = 0) const override;
    virtual int generateDot(std::ostream& out, int& currentId) const override;
};