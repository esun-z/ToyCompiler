// node.cpp
#include "node.h"
#include "parser.hpp" // 包含 token 定义

// NInteger 的 print 实现
void NInteger::print(int indent) const {
    std::cout << value;
}

// NDouble 的 print 实现
void NDouble::print(int indent) const {
    std::cout << value;
}

// NIdentifier 的 print 实现
void NIdentifier::print(int indent) const {
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
void NBinaryOperator::print(int indent) const {
    lhs.print();
    switch(op) {
        case TPLUS: std::cout << " + "; break;
        case TMINUS: std::cout << " - "; break;
        case TMUL: std::cout << " * "; break;
        case TDIV: std::cout << " / "; break;
        case TCEQ: std::cout << " == "; break;
        case TCNE: std::cout << " != "; break;
        case TCLT: std::cout << " < "; break;
        case TCLE: std::cout << " <= "; break;
        case TCGT: std::cout << " > "; break;
        case TCGE: std::cout << " >= "; break;
        case TMOD: std::cout << " %"; break;
        default: std::cout << " op(" << op << ") "; break;
    }
    rhs.print();
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
        for(int i = 0; i < indent + 2; ++i) std::cout << " ";
        stmt->print(indent + 2);
        std::cout << "\n";
    }
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "}";
}

// NExpressionStatement 的 print 实现
void NExpressionStatement::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    expression.print();
    std::cout << ";";
}

// NReturnStatement 的 print 实现
void NReturnStatement::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "return ";
    expression.print();
    std::cout << ";";
}

// NVariableDeclaration 的 print 实现
void NVariableDeclaration::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    type.print();
    std::cout << " ";
    id.print();
    if(assignmentExpr) {
        std::cout << " = ";
        assignmentExpr->print();
    }
    std::cout << ";";
}

// NConstDeclaration 的 print 实现
void NConstDeclaration::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "const ";
    type.print();
    std::cout << " ";
    id.print();
    if(assignmentExpr) {
        std::cout << " = ";
        assignmentExpr->print();
    }
    std::cout << ";";
}

// NFunctionDeclaration 的 print 实现
void NFunctionDeclaration::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    type.print();
    std::cout << " ";
    id.print();
    std::cout << "(";
    for(size_t i = 0; i < arguments.size(); ++i) {
        arguments[i]->type.print();
        std::cout << " ";
        arguments[i]->id.print();
        if(i != arguments.size() - 1)
            std::cout << ", ";
    }
    std::cout << ") ";
    block.print(indent);
}

// NIfStatement 的 print 实现
void NIfStatement::print(int indent) const {
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
void NWhileStatement::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "while (";
    condition.print();
    std::cout << ") ";
    block.print(indent);
}
