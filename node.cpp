// node.cpp
#include "node.h"
#include "parser.hpp" // 包含 token 定义
#include <fstream>
#include <map>
#include <typeinfo>  // 包含 typeid 的头文件



void Node::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "$";
}

void NCompUnit::print(int indent) const {
    for(auto decl : decls) {
        decl->print(indent);
        std::cout << "\n";
    }
}

// NInteger 的 print 实现
void NInteger::print(int indent) const {
    std::cout << value;
}

// NDouble 的 print 实现
void NFloat::print(int indent) const {
    std::cout << value;
}

// NIdentifier 的 print 实现
void NIdent::print(int indent) const {
    switch (type)
    {
    case TINTTYPE:
        std::cout << "int ";
        break;
    case TFLOATTYPE:
        std::cout << "float ";
        break;
    case TVOIDTYPE:
        std::cout << "void ";
        break;
    default:
        break;
    }
    std::cout << name;
}

// NMethodCall 的 print 实现
void NMethodCall::print(int indent) const {
    id.print();
    std::cout << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        arguments[i]->print();
        if (i != arguments.size() - 1)
            std::cout << ", ";
    }
    std::cout << ")";
}

// NBinaryOperator 的 print 实现
void NBinaryExpr::print(int indent) const {
    lhs.print();
    switch(op) {
        case TPLUS: std::cout << " + "; break;
        case TMINUS: std::cout << " - "; break;
        case TMUL: std::cout << " * "; break;
        case TDIV: std::cout << " / "; break;
        case TMOD: std::cout << " %"; break;
        default: std::cout << " op(" << op << ") "; break;
    }
    rhs.print();
}

void NLogicalBinaryExpr::print(int ident) const {
    lhs.print();
    switch (op)
    {
    case TCEQ:
        std::cout << " == ";
        break;
    case TCNE:
        std::cout << " != ";
        break;
    case TCLT:
        std::cout << " < ";
        break;
    case TCLE:
        std::cout << " <= ";
        break;
    case TCGT:
        std::cout << " > ";
        break;
    case TCGE:
        std::cout << " >= ";
        break;
    case TAND:
        std::cout << " && ";
        break;
    case TOR:
        std::cout << " || ";
        break;
    default:
        std::cout << " op(" << op << ") ";
        break;
    }
    rhs.print();
}

void NUnaryExpr::print(int indent) const {
    switch (op)
    {
    case TMINUS:
        std::cout << "-";
        break;
    default:
        std::cout << " op(" << op << ") ";
        break;
    }
    expr.print();
}

void NLogicalUnaryExpr::print(int indent) const {
    switch (op)
    {
    case TNOT:
        std::cout << "!";
        break;
    
    default:
        std::cout << " op(" << op << ") ";
        break;
    }
}

// NAssignment 的 print 实现
void NAssignment::print(int indent) const {
    lhs.print();
    std::cout << " = ";
    rhs.print();
}

// NBlock 的 print 实现
void NBlock::print(int indent) const {
    std::cout << "{\n";
    for(auto stmt : statements) {
        stmt->print(indent + 2);
        std::cout << "\n";
    }
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "}";
}

// NExpressionStatement 的 print 实现
void NExprStmt::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    expression.print();
    std::cout << ";";
}

// NReturnStatement 的 print 实现
void NReturnStmt::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "return ";
    expression.print();
    std::cout << ";";
}



// NVariableDeclaration 的 print 实现
void NVarDecl::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    if(isConst) std::cout << "const ";
    id.print();
    if(assignmentExpr) {
        std::cout << " = ";
        assignmentExpr->print();
    }
    std::cout << ";";
}

// NConstDeclaration 的 print 实现
// void NConstDeclaration::print(int indent) const {
//     for(int i = 0; i < indent; ++i) std::cout << " ";
//     std::cout << "const ";
//     type.print();
//     std::cout << " ";
//     id.print();
//     if(assignmentExpr) {
//         std::cout << " = ";
//         assignmentExpr->print();
//     }
//     std::cout << ";";
// }

// NFunctionDeclaration 的 print 实现
void NFuncDecl::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    id.print();
    std::cout << "(";
    for(size_t i = 0; i < arguments.size(); ++i) {
        arguments[i]->id.print();
        if(i != arguments.size() - 1)
            std::cout << ", ";
    }
    std::cout << ") ";
    block.print(indent); 
}

// NIfStatement 的 print 实现
void NIfStmt::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "if (";
    condition.print();
    std::cout << ") ";
    trueBlock.print(indent);
    if (falseBlock) {
        std::cout << "\n";
        for(int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "else ";
        falseBlock->print(indent);
    }
}

// NWhileStatement 的 print 实现
void NWhileStmt::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "while (";
    condition.print();
    std::cout << ") ";
    block.print(indent);
}
///////////////////////////////////////////////////


// Helper function to escape quotes in labels
std::string escapeQuotes(const std::string& str) {
    std::string escaped;
    for(char c : str) {
        if(c == '\"') escaped += "\\\"";
        else escaped += c;
    }
    return escaped;
}

// 基类 Node 的默认实现（如果有需要，可以保留或删除）
int Node::generateDot(std::ostream& out, int& currentId) const {
    // 基类不直接实例化，返回-1
    return -1;
}

// NCompUnit 的 generateDot 实现
int NCompUnit::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NCompUnit\", shape=rectangle];\n";
    for(auto decl : decls) {
        int childId = decl->generateDot(out, currentId);
        if(childId != -1) {
            out << "  node" << myId << " -> node" << childId << ";\n";
                     
            // 根据 stmt 的具体类型进行判断
            if (typeid(*decl) == typeid(NFuncDecl)) {
                // 逻辑1: 当 stmt 是 NFuncDecl 类型时不打印分号
            }
            else {
                // 其他类型时打印分号
                // 分号
                int semicolonId = currentId++;
                out << "  node" << semicolonId << " [label=\";\", shape=ellipse];\n";
                out << "  node" << myId << " -> node" << semicolonId << ";\n";
            }
        }
    }
    return myId;
}


// NVarDecl 的 generateDot 实现
int NVarDecl::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NVarDecl\", shape=rectangle];\n";
    if(isConst){
        // const符号
        int constId = currentId++;
        out << "  node" << constId << " [label=\"const\", shape=ellipse];\n";
        out << "  node" << myId << " -> node" << constId << ";\n";
        
    }
    // 类型节点
    int typeId = currentId++;
    std::string typeStr;
    switch(id.type) {
        case TINTTYPE:
            typeStr = "int";
            break;
        case TFLOATTYPE:
            typeStr = "float";
            break;
        case TVOIDTYPE:
            typeStr = "void";
            break;
        default:
            typeStr = "unknown";
            break;
    }
    out << "  node" << typeId << " [label=\"" << typeStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << typeId << ";\n";
    
    // 标识符节点
    int idId = id.generateDot(out, currentId);
    if(idId != -1) {
        out << "  node" << myId << " -> node" << idId << ";\n";
    }
    
    // 赋值操作符和赋值表达式
    if(assignmentExpr) {
        // 赋值操作符节点
        int assignOpId = currentId++;
        out << "  node" << assignOpId << " [label=\"=\", shape=ellipse];\n";
        out << "  node" << myId << " -> node" << assignOpId << ";\n";
        
        // 赋值表达式节点
        int exprId = assignmentExpr->generateDot(out, currentId);
        if(exprId != -1) {
            out << "  node" << myId << " -> node" << exprId << ";\n";
        }
    }
    
    return myId;
}



// NFuncDecl 的 generateDot 实现
int NFuncDecl::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NFuncDecl\", shape=rectangle];\n";
    // 类型节点
    int typeId = currentId++;
    std::string typeStr;
    switch(id.type) {
        case TINTTYPE:
            typeStr = "int";
            break;
        case TFLOATTYPE:
            typeStr = "float";
            break;
        case TVOIDTYPE:
            typeStr = "void";
            break;
        default:
            typeStr = "unknown";
            break;
    }
    out << "  node" << typeId << " [label=\"" << typeStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << typeId << ";\n";
    
    // 函数名
    int idId = id.generateDot(out, currentId);
    if(idId != -1) {
        out << "  node" << myId << " -> node" << idId << ";\n";
    }
    // 左圆括号
    int leftBracketId = currentId++;
    out << "  node" << leftBracketId << " [label=\"(\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << leftBracketId << ";\n";
    
    // 参数列表
    int numArguments = arguments.size();  // 获取参数总数
    int currentIndex = 0;  // 用于跟踪当前是第几个参数
    // 参数列表
    for(const auto& arg : arguments) {
        int argId = arg->generateDot(out, currentId);
        if(argId != -1) {
            out << "  node" << myId << " -> node" << argId << ";\n";
            currentIndex++;
            if (currentIndex < numArguments) {
                // 如果不是最后一个参数，添加逗号
                int commaId = currentId++;
                out << "  node" << commaId << " [label=\",\", shape=ellipse];\n";
                out << "  node" << myId << " -> node" << commaId << ";\n";
                
            }
        }
    }
    // 右圆括号
    int rightBracketId = currentId++;
    out << "  node" << rightBracketId << " [label=\")\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << rightBracketId << ";\n";
    
    // 函数体
    int blockId = block.generateDot(out, currentId);
    if(blockId != -1) {
        out << "  node" << myId << " -> node" << blockId << ";\n";
    }
    return myId;
}

// NInteger 的 generateDot 实现
int NInteger::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"" << value << "\", shape=ellipse];\n";
    return myId;
}

// NFloat 的 generateDot 实现
int NFloat::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"" << value << "\", shape=ellipse];\n";
    return myId;
}

// NIdent 的 generateDot 实现
int NIdent::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"" << escapeQuotes(name) << "\", shape=ellipse];\n";
    return myId;
}

// NMethodCall 的 generateDot 实现
int NMethodCall::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NMethodCall\", shape=rectangle];\n";
    // 方法名
    int idId = id.generateDot(out, currentId);
    if(idId != -1) {
        out << "  node" << myId << " -> node" << idId << ";\n";
    }
    // 左圆括号
    int leftBracketId = currentId++;
    out << "  node" << leftBracketId << " [label=\"(\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << leftBracketId << ";\n";
    
    // 参数列表
    int numArguments = arguments.size();  // 获取参数总数
    int currentIndex = 0;  // 用于跟踪当前是第几个参数
    // 参数列表
    for(const auto& arg : arguments) {
        int argId = arg->generateDot(out, currentId);
        if(argId != -1) {
            out << "  node" << myId << " -> node" << argId << ";\n";
            currentIndex++;
            if (currentIndex < numArguments) {
                // 如果不是最后一个参数，添加逗号
                int commaId = currentId++;
                out << "  node" << commaId << " [label=\",\", shape=ellipse];\n";
                out << "  node" << myId << " -> node" << commaId << ";\n";
                
            }
        }
    }
    
    // 右圆括号
    int rightBracketId = currentId++;
    out << "  node" << rightBracketId << " [label=\")\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << rightBracketId << ";\n";


    return myId;
}

// NBinaryExpr 的 generateDot 实现
int NBinaryExpr::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NBinaryExpr\", shape=rectangle];\n";

    // 左子表达式
    int lhsId = lhs.generateDot(out, currentId);
    if(lhsId != -1) {
        out << "  node" << myId << " -> node" << lhsId << ";\n";
    }

    // 操作符叶子节点
    int opId = currentId++;
    std::string opStr;
    switch(op) {
        case TPLUS:  opStr = "+"; break;
        case TMINUS: opStr = "-"; break;
        case TMUL:   opStr = "*"; break;
        case TDIV:   opStr = "/"; break;
        case TMOD:   opStr = "%"; break;
        default:     opStr = "op"; break;
    }
    out << "  node" << opId << " [label=\"" << opStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << opId << ";\n";


    // 右子表达式
    int rhsId = rhs.generateDot(out, currentId);
    if(rhsId != -1) {
        out << "  node" << myId << " -> node" << rhsId << ";\n";
    }

    return myId;
}

// NLogicalBinaryExpr 的 generateDot 实现
int NLogicalBinaryExpr::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NLogicalBinaryExpr\", shape=rectangle];\n";

    // 左子表达式
    int lhsId = lhs.generateDot(out, currentId);
    if(lhsId != -1) {
        out << "  node" << myId << " -> node" << lhsId << ";\n";
    }


    // 操作符叶子节点
    int opId = currentId++;
    std::string opStr;
    switch(op) {
        case TCEQ: opStr = "=="; break;
        case TCNE: opStr = "!="; break;
        case TCLT: opStr = "<"; break;
        case TCLE: opStr = "<="; break;
        case TCGT: opStr = ">"; break;
        case TCGE: opStr = ">="; break;
        case TAND: opStr = "&&"; break;
        case TOR:  opStr = "||"; break;
        default:   opStr = "op"; break;
    }
    out << "  node" << opId << " [label=\"" << opStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << opId << ";\n";


    // 右子表达式
    int rhsId = rhs.generateDot(out, currentId);
    if(rhsId != -1) {
        out << "  node" << myId << " -> node" << rhsId << ";\n";
    }

    return myId;
}

// NUnaryExpr 的 generateDot 实现
int NUnaryExpr::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NUnaryExpr\", shape=rectangle];\n";

    // 操作符叶子节点
    int opId = currentId++;
    std::string opStr;
    switch(op) {
        case TMINUS: opStr = "-"; break;
        // 添加更多操作符如 TNOT 等
        default:     opStr = "op"; break;
    }
    out << "  node" << opId << " [label=\"" << opStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << opId << ";\n";

    // 子表达式
    int exprId = expr.generateDot(out, currentId);
    if(exprId != -1) {
        out << "  node" << myId << " -> node" << exprId << ";\n";
    }

    return myId;
}

// NLogicalUnaryExpr 的 generateDot 实现
int NLogicalUnaryExpr::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NLogicalUnaryExpr\", shape=rectangle];\n";

    // 操作符叶子节点
    int opId = currentId++;
    std::string opStr;
    switch(op) {
        case TNOT: opStr = "!"; break;
        // 添加更多操作符如其他逻辑操作符
        default:   opStr = "op"; break;
    }
    out << "  node" << opId << " [label=\"" << opStr << "\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << opId << ";\n";

    // 子表达式
    int exprId = expr.generateDot(out, currentId);
    if(exprId != -1) {
        out << "  node" << myId << " -> node" << exprId << ";\n";
    }

    return myId;
}

// NAssignment 的 generateDot 实现
int NAssignment::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NAssignment\", shape=rectangle];\n";
    
    // 左值
    int lhsId = lhs.generateDot(out, currentId);
    if(lhsId != -1) {
        out << "  node" << myId << " -> node" << lhsId << ";\n";
    }
    // =符号
    int equalSignId = currentId++;
    out << "  node" << equalSignId << " [label=\"=\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << equalSignId << ";\n";
    

    // 右值
    int rhsId = rhs.generateDot(out, currentId);
    if(rhsId != -1) {
        out << "  node" << myId << " -> node" << rhsId << ";\n";
    }

    return myId;
}

// NBlock 的 generateDot 实现
int NBlock::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NBlock\", shape=rectangle];\n";
    // 左花括号
    int leftBraceId = currentId++;
    out << "  node" << leftBraceId << " [label=\"{\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << leftBraceId << ";\n";
    // 语句列表
    
    for(auto stmt : statements) {
        int stmtId = stmt->generateDot(out, currentId);
        if(stmtId != -1) {
            out << "  node" << myId << " -> node" << stmtId << ";\n";
            
            // 根据 stmt 的具体类型进行判断
            if (typeid(*stmt) == typeid(NBlock)) {
                // 逻辑1: 当 stmt 是 NBlock 类型时不打印分号
            }
            else if (typeid(*stmt) == typeid(NIfStmt)) {
                // 逻辑2: 当 stmt 是 NIfStmt 类型时不打印分号
            }
            else if (typeid(*stmt) == typeid(NWhileStmt)) {
                // 逻辑3: 当 stmt 是 NWhileStmt 类型时不打印分号
            }
            else {
                // 其他类型时打印分号
                // 分号
                int semicolonId = currentId++;
                out << "  node" << semicolonId << " [label=\";\", shape=ellipse];\n";
                out << "  node" << myId << " -> node" << semicolonId << ";\n";
            }
         
        }
    }
    // 右花括号
    int rightBraceId = currentId++;
    out << "  node" << rightBraceId << " [label=\"}\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << rightBraceId << ";\n";
    return myId;
}

// NExprStmt 的 generateDot 实现
int NExprStmt::generateDot(std::ostream& out, int& currentId) const {
    return expression.generateDot(out, currentId);
}

// NReturnStmt 的 generateDot 实现
int NReturnStmt::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NReturnStmt\", shape=rectangle];\n";
    
    // return符号
    int returnId = currentId++;
    out << "  node" << returnId << " [label=\"return\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << returnId << ";\n";
    
    // 返回表达式
    int exprId = expression.generateDot(out, currentId);
    if(exprId != -1) {
        out << "  node" << myId << " -> node" << exprId << ";\n";
    }
    
    return myId;
}

// NIfStmt 的 generateDot 实现
int NIfStmt::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NIfStmt\", shape=rectangle];\n";

    // if符号
    int ifId = currentId++;
    out << "  node" << ifId << " [label=\"if\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << ifId << ";\n";
    
    // 左圆括号
    int leftBracketId = currentId++;
    out << "  node" << leftBracketId << " [label=\"(\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << leftBracketId << ";\n";
    
    // 条件表达式
    int condId = condition.generateDot(out, currentId);
    if(condId != -1) {
        out << "  node" << myId << " -> node" << condId << ";\n";
    }
    // 右圆括号
    int rightBracketId = currentId++;
    out << "  node" << rightBracketId << " [label=\")\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << rightBracketId << ";\n";
    

    // 真分支
    int trueBlockId = trueBlock.generateDot(out, currentId);
    if(trueBlockId != -1) {
        out << "  node" << myId << " -> node" << trueBlockId << ";\n";
    }

    // 假分支（如果存在）
    if(falseBlock) {
        // else符号
        int elseId = currentId++;
        out << "  node" << elseId << " [label=\"else\", shape=ellipse];\n";
        out << "  node" << myId << " -> node" << elseId << ";\n";
        
        int falseBlockId = falseBlock->generateDot(out, currentId);
        if(falseBlockId != -1) {
            out << "  node" << myId << " -> node" << falseBlockId << ";\n";
        }
    }

    return myId;
}

// NWhileStmt 的 generateDot 实现
int NWhileStmt::generateDot(std::ostream& out, int& currentId) const {
    int myId = currentId++;
    out << "  node" << myId << " [label=\"NWhileStmt\", shape=rectangle];\n";
    // while符号
    int whileId = currentId++;
    out << "  node" << whileId << " [label=\"while\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << whileId << ";\n";
    // 左圆括号
    int leftBracketId = currentId++;
    out << "  node" << leftBracketId << " [label=\"(\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << leftBracketId << ";\n";
    
    // 条件表达式
    int condId = condition.generateDot(out, currentId);
    if(condId != -1) {
        out << "  node" << myId << " -> node" << condId << ";\n";
    }
    // 右圆括号
    int rightBracketId = currentId++;
    out << "  node" << rightBracketId << " [label=\")\", shape=ellipse];\n";
    out << "  node" << myId << " -> node" << rightBracketId << ";\n";
    

    // 循环体
    int blockId = block.generateDot(out, currentId);
    if(blockId != -1) {
        out << "  node" << myId << " -> node" << blockId << ";\n";
    }

    return myId;
}
