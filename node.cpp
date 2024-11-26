#include "node.h"
#include "parser.hpp" // 包含 token 定义

// 整数节点的 print 实现
void NInteger::print(int indent) const {
    std::cout << value;
}

// 双精度浮点数节点的 print 实现
void NDouble::print(int indent) const {
    std::cout << value;
}

// 标识符节点的 print 实现
void NIdentifier::print(int indent) const {
    std::cout << name;
}

// 方法调用节点的 print 实现
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

// 二元运算符节点的 print 实现
void NBinaryOperator::print(int indent) const {
    lhs.print();
    switch(op) {
        case TPLUS: std::cout << " + "; break;
        case TMINUS: std::cout << " - "; break;
        case TMUL: std::cout << " * "; break;
        case TDIV: std::cout << " / "; break;
        default: std::cout << " ? "; break;
    }
    rhs.print();
}

// 赋值节点的 print 实现
void NAssignment::print(int indent) const {
    lhs.print();
    std::cout << " = ";
    rhs.print();
}

// 块节点的 print 实现
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

// 表达式语句节点的 print 实现
void NExpressionStatement::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    expression.print();
    std::cout << ";";
}

// 返回语句节点的 print 实现
void NReturnStatement::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "return ";
    expression.print();
    std::cout << ";";
}

// 变量声明节点的 print 实现
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

// 外部声明节点的 print 实现
void NExternDeclaration::print(int indent) const {
    for(int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "extern ";
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
    std::cout << ");";
}

// 函数声明节点的 print 实现
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
