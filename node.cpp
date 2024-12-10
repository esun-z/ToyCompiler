// node.cpp
#include "node.h"
#include "parser.hpp" // 包含 token 定义

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
    block.print(indent); // TODO: 缩进似乎存在问题
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
