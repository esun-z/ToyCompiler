#include <stack>
#include <typeinfo>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include "llvm/Pass.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitstream/BitstreamReader.h>
#include <llvm/Bitstream/BitstreamWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class NCompUnit;

static LLVMContext MyContext;

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    std::map<std::string, Value*> globals;
    std::map<std::string, Value*> mergedLocals;
    Module *module;
    CodeGenContext() { module = new Module("main", MyContext); }
    
    void generateCode(NCompUnit& root);
    GenericValue runCode();
    std::map<std::string, Value*>& locals() {
        // merge all locals from all blocks
        // mergedLocals.clear();
        // std::stack<CodeGenBlock*> buf;
        // while(!blocks.empty()) {
        //     auto locals = blocks.top()->locals;
        //     for(auto it = locals.begin(); it != locals.end(); ++it) {
        //         mergedLocals[it->first] = it->second;
        //     }
        //     buf.push(blocks.top());
        //     blocks.pop();
        // }

        // while(!buf.empty()) {
        //     blocks.push(buf.top());
        //     buf.pop();
        // }
        // return mergedLocals;
        return blocks.top()->locals;
    }
    BasicBlock *currentBlock() { 
        if(blocks.empty()) {
            return NULL;
        }
        return blocks.top()->block; 
    }
    void pushBlock(BasicBlock *block) {
        bool notFirstBlock = false;
        if(notFirstBlock && blocks.top() && blocks.top()->locals.size() > 0) {
            auto locals = blocks.top()->locals;
            blocks.push(new CodeGenBlock()); 
            blocks.top()->returnValue = NULL; 
            blocks.top()->block = block; 
            // copy locals
            for(auto it = locals.begin(); it != locals.end(); ++it) {
                blocks.top()->locals[it->first] = it->second;
            }
        }
        else {
            blocks.push(new CodeGenBlock()); 
            blocks.top()->returnValue = NULL; 
            blocks.top()->block = block;
        }
    }
    void popBlock() { CodeGenBlock *top = blocks.top(); blocks.pop(); delete top; }
    void setCurrentReturnValue(Value *value) { blocks.top()->returnValue = value; }
    Value* getCurrentReturnValue() { return blocks.top()->returnValue; }
};
