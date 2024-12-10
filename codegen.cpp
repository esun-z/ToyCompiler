#include "node.h"
#include "codegen.h"
#include "parser.hpp"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NCompUnit& root)
{
	std::cout << "Generating code...\n";
	
	/* Create the top level interpreter function to call as entry */
	// esun: must create a main bb though there is main func
	//vector<Type*> argTypes;
	//FunctionType *ftype = FunctionType::get(Type::getVoidTy(MyContext), makeArrayRef(argTypes), false);
	//mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "program", module);
	//BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", mainFunction, 0);
	
	/* Push a new variable/block context */
	//pushBlock(bblock);

	root.codeGen(*this); /* emit bytecode for the toplevel block */
	// ReturnInst::Create(MyContext, bblock);
	// popBlock();
	
	/* Print the bytecode in a human-readable format 
	   to see if our program compiled properly
	 */
	std::cout << "Code is generated.\n";
	// module->dump();

	legacy::PassManager pm;
	// TODO:
	pm.add(createPrintModulePass(outs()));
	pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder( unique_ptr<Module>(module) ).create();
	if (!ee) {
		std:cerr << "Failed to create Execution Engine." << std::endl;
		exit(1);
	}
	ee->finalizeObject();
	// init all global variables
	ee->runStaticConstructorsDestructors(false);

	// find a function named main
	Function *mainFunction = module->getFunction("main");
	if (!mainFunction) {
		std::cerr << "Function main not found." << std::endl;
		exit(1);
	}

	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(mainFunction, noargs);
	std::cout << "Code was run.\n";
	return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdent& type) 
{
	if (type.type == TINTTYPE) {
		return Type::getInt64Ty(MyContext);
	}
	else if (type.type == TFLOATTYPE) {
		return Type::getDoubleTy(MyContext);
	}
	return Type::getVoidTy(MyContext);
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context)
{
	std::cout << "Creating integer: " << value << endl;
	return ConstantInt::get(Type::getInt64Ty(MyContext), value, true);
}

Value* NFloat::codeGen(CodeGenContext& context)
{
	std::cout << "Creating double: " << value << endl;
	return ConstantFP::get(Type::getDoubleTy(MyContext), value);
}

Value* NIdent::codeGen(CodeGenContext& context)
{
	std::cout << "Creating identifier reference: " << name << endl;

	if (context.locals().find(name) != context.locals().end()) {
		return new LoadInst(context.locals()[name]->getType(), context.locals()[name], name, false, context.currentBlock());
	} else if (context.module->getGlobalVariable(name.c_str())) {
		auto gvar = context.module->getNamedGlobal(name.c_str());
		IRBuilder<> builder(context.currentBlock());
		return builder.CreateLoad(gvar->getType()->getPointerElementType(), gvar, "");
	}
	else {
		std::cerr << "undeclared variable " << name << endl;
		return NULL;
	}

	
	// if (context.locals().find(name) == context.locals().end()) {
	// 	std::cerr << "undeclared variable " << name << endl;
	// 	return NULL;
	// }

	// // return nullptr;  
	// return new LoadInst(context.locals()[name]->getType(),context.locals()[name], name, false, context.currentBlock());
}

Value* NMethodCall::codeGen(CodeGenContext& context)
{
	Function *function = context.module->getFunction(id.name.c_str());
	if (function == NULL) {
		std::cerr << "no such function " << id.name << endl;
	}
	std::vector<Value*> args;
	ExprList::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		args.push_back((**it).codeGen(context));
	}
	CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
	std::cout << "Creating method call: " << id.name << endl;
	return call;
}

Value* NBinaryExpr::codeGen(CodeGenContext& context)
{

	std::cout << "Creating binary operation " << op << endl;
	Instruction::BinaryOps instr;
	switch (op) {
		case TPLUS: 	instr = Instruction::Add; goto math;
		case TMINUS: 	instr = Instruction::Sub; goto math;
		case TMUL: 		instr = Instruction::Mul; goto math;
		case TDIV: 		instr = Instruction::SDiv; goto math;
				
		case TMOD: {
			Function *modf = context.module->getFunction("mod");
			if (modf == NULL) {
				std::vector<Type*> argTypes;
				argTypes.push_back(Type::getInt64Ty(MyContext));
				argTypes.push_back(Type::getInt64Ty(MyContext));
				FunctionType *ftype = FunctionType::get(Type::getInt64Ty(MyContext), makeArrayRef(argTypes), false);
				modf = Function::Create(ftype, GlobalValue::ExternalLinkage, "mod", context.module);
				modf->setCallingConv(CallingConv::C);
			}
			std::vector<Value*> args;
			args.push_back(lhs.codeGen(context));
			args.push_back(rhs.codeGen(context));
			return CallInst::Create(modf, makeArrayRef(args), "", context.currentBlock());
		}
	}
	return NULL;
math:
	return BinaryOperator::Create(instr, lhs.codeGen(context), 
		rhs.codeGen(context), "", context.currentBlock());
}

Value* NLogicalBinaryExpr::codeGen(CodeGenContext& context)
{
	std::cout << "Creating logical binary operation " << op << endl;
	Instruction::BinaryOps instr;
	CmpInst::Predicate pred;
	switch (op) {
		case TAND: 	instr = Instruction::And; goto logic;
		case TOR: 	instr = Instruction::Or; goto logic;
		case TCEQ: 	pred = CmpInst::ICMP_EQ; goto cmp;
		case TCNE: 	pred = CmpInst::ICMP_NE; goto cmp;
		case TCLT: 	pred = CmpInst::ICMP_SLT; goto cmp;
		case TCLE: 	pred = CmpInst::ICMP_SLE; goto cmp;
		case TCGT: 	pred = CmpInst::ICMP_SGT; goto cmp;
		case TCGE: 	pred = CmpInst::ICMP_SGE; goto cmp;
	}
	return NULL;
cmp:
	return CmpInst::Create(Instruction::ICmp, pred, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
logic:
	return BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
}

Value* NUnaryExpr::codeGen(CodeGenContext& context)
{
	std::cout << "Creating unary operation " << op << endl;
	Instruction::UnaryOps instr;
	switch (op) {
		case TMINUS: 	instr = Instruction::UnaryOps::FNeg; goto math;
	}
	return NULL;
math:
	return UnaryOperator::Create(instr, expr.codeGen(context), "", context.currentBlock());
}

Value* NLogicalUnaryExpr::codeGen(CodeGenContext& context)
{
	std::cout << "Creating logical unary operation " << op << endl;
	llvm::IRBuilder<> builder(context.currentBlock());
	switch (op) {
		case TNOT: {
			llvm::Value *val = expr.codeGen(context);
			return builder.CreateNot(val, "not");
		}
	}
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
	std::cout << "Creating assignment for " << lhs.name << endl;
	
	if (context.locals().find(lhs.name) != context.locals().end()) {
		return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
	} else if (context.module->getGlobalVariable(lhs.name.c_str())) {
		// do assign here
		IRBuilder<> builder(context.currentBlock());
		GlobalVariable *gvar = context.module->getNamedGlobal(lhs.name.c_str());
		if (gvar == NULL) {
			std::cerr << "no such global variable " << lhs.name << endl;
		}
		return builder.CreateStore(rhs.codeGen(context), gvar);
	}
	else {
		std::cerr << "undeclared variable " << lhs.name << endl;
		return NULL;
	}
}

Value* NCompUnit::codeGen(CodeGenContext& context)
{
	DeclList::const_iterator it;
	Value *last = NULL;
	for (it = decls.begin(); it != decls.end(); it++) {
		std::cout << "Generating code for " << typeid(**it).name() << endl;
		last = (**it).codeGen(context);
	}
	std::cout << "Creating CompUnit" << endl;
	return last;
}

Value* NBlock::codeGen(CodeGenContext& context)
{
	StmtList::const_iterator it;
	Value *last = NULL;
	for (it = statements.begin(); it != statements.end(); it++) {
		std::cout << "Generating code for " << typeid(**it).name() << endl;
		last = (**it).codeGen(context);
	}
	std::cout << "Creating block" << endl;
	return last;
}

Value* NExprStmt::codeGen(CodeGenContext& context)
{
	std::cout << "Generating code for " << typeid(expression).name() << endl;
	return expression.codeGen(context);
}

Value* NReturnStmt::codeGen(CodeGenContext& context)
{
	std::cout << "Generating return code for " << typeid(expression).name() << endl;
	Value *returnValue = expression.codeGen(context);
	context.setCurrentReturnValue(returnValue);
	return returnValue;
}

Value* NVarDecl::codeGen(CodeGenContext& context)
{
	std::cout << "Creating variable declaration " << id.type << " " << id.name << endl;

	// if current block is null, then it is a global variable
	if (context.currentBlock() == NULL) {
		std::cout << "Creating global variable " << id.name << endl;
		// Global variable
		// GlobalVariable *gvar = new GlobalVariable(*context.module, typeOf(id), false, GlobalValue::InternalLinkage, NULL, id.name.c_str());
		// if (assignmentExpr != NULL)
		// {
		// 	gvar->setInitializer(static_cast<llvm::Constant*>(assignmentExpr->codeGen(context)));
		// }
		context.module->getOrInsertGlobal(id.name.c_str(), typeOf(id));
		GlobalVariable *gvar = context.module->getNamedGlobal(id.name.c_str());
		gvar->setLinkage(GlobalValue::CommonLinkage);
		if (assignmentExpr != NULL)
		{
			// NAssignment assn(id, *assignmentExpr);
			// std::cout << "gen assn" << std::endl;
			// Value* result = assn.codeGen(context);
			// std::cout << "ready for dyn_cast" << std::endl;

			// auto constant = llvm::dyn_cast<llvm::Constant>(assignmentExpr->codeGen(context));
			// // print out constant
			// std::cout << "constant: ";
			// constant->print(outs());
			// std::cout << "constant end" << std::endl;
			gvar->setInitializer(llvm::dyn_cast<llvm::Constant>(assignmentExpr->codeGen(context)));

			NInteger *intVal = dynamic_cast<NInteger*>(assignmentExpr);
			NFloat *floatVal = dynamic_cast<NFloat*>(assignmentExpr);

			context.globals.insert(std::pair<std::string, Value*>(id.name, assignmentExpr->codeGen(context)));

			if (intVal) {
				gvar->setInitializer(llvm::ConstantInt::get(MyContext, llvm::APInt(64, intVal->value, true)));
				context.globals.insert(std::pair<std::string, Constant*>(id.name, llvm::ConstantInt::get(MyContext, llvm::APInt(64, intVal->value, true))));
			}
			else if (floatVal) {
				gvar->setInitializer(llvm::ConstantFP::get(MyContext, llvm::APFloat(floatVal->value)));
				context.globals.insert(std::pair<std::string, Constant*>(id.name, llvm::ConstantFP::get(MyContext, llvm::APFloat(floatVal->value))));
			}
			else {
				gvar->setInitializer(llvm::dyn_cast<llvm::Constant>(assignmentExpr->codeGen(context)));
				context.globals.insert(std::pair<std::string, Constant*>(id.name, llvm::dyn_cast<llvm::Constant>(assignmentExpr->codeGen(context))));
			}
		}
		// context.globals.insert(std::pair<std::string, Value*>(id.name, gvar));
		// std::cout << "gb1" << std::endl;
		// // context.locals()[id.name] = gvar;
		// std::cout << "gb2" << std::endl;
		return gvar;
	}
	else {
		AllocaInst *alloc = new AllocaInst(typeOf(id),8, id.name.c_str(), context.currentBlock());
		context.locals()[id.name] = alloc;
		if (assignmentExpr != NULL)
		{
			NAssignment assn(id, *assignmentExpr);
			assn.codeGen(context);
		}
		return alloc;
	}
}

Value* NFuncDecl::codeGen(CodeGenContext& context)
{
	vector<Type*> argTypes;
	VariableList::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		argTypes.push_back(typeOf((**it).id));
	}
	FunctionType *ftype = FunctionType::get(typeOf(id), makeArrayRef(argTypes), false);
	Function *function;
	// check if is main
	if (id.name == "main") {
		function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);

		// init all global variables
		// for (auto& globalVar : context.globals) {
        //     // Assuming globalVariables is a map of variable names to their initial values
        //     Value* initValue = globalVar.second;
        //     if (initValue) {
		// 		GlobalVariable* gvar = context.module->getNamedGlobal(globalVar.first);
		// 		if (gvar) {
		// 			gvar->setInitializer(initValue);
		// 		}
		// 	}
        // }
	}
	else {
		function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
	}
	BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", function, 0);

	context.pushBlock(bblock);

	Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;

	for (it = arguments.begin(); it != arguments.end(); it++) {
		(**it).codeGen(context);
		
		argumentValue = &*argsValues++;
		argumentValue->setName((*it)->id.name.c_str());
		StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
	}
	
	block.codeGen(context);
	ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

	context.popBlock();
	std::cout << "Creating function: " << id.name << endl;
	return function;
}

Value* NWhileStmt::codeGen(CodeGenContext& context)
{
    Function *function = context.currentBlock()->getParent();
    BasicBlock *condBB = BasicBlock::Create(MyContext, "whileCond", function);
    BasicBlock *loopBB = BasicBlock::Create(MyContext, "whileLoop", function);
    BasicBlock *afterBB = BasicBlock::Create(MyContext, "whileEnd", function);

    // Branch to the condition block
    IRBuilder<> builder(context.currentBlock());
    builder.CreateBr(condBB);

    // Generate code for the condition
    context.pushBlock(condBB);
    Value *condValue = condition.codeGen(context);
    builder.SetInsertPoint(condBB);
    builder.CreateCondBr(condValue, loopBB, afterBB);
    context.popBlock();

    // Generate code for the loop body
    context.pushBlock(loopBB);
    block.codeGen(context);
    builder.SetInsertPoint(loopBB);
    builder.CreateBr(condBB);
    context.popBlock();

    // Set the insertion point to the after block
    context.pushBlock(afterBB);
    builder.SetInsertPoint(afterBB);

    return nullptr;
}

Value* NIfStmt::codeGen(CodeGenContext& context)
{
    Function *function = context.currentBlock()->getParent();
    BasicBlock *thenBB = BasicBlock::Create(MyContext, "then", function);
    BasicBlock *elseBB = falseBlock ? BasicBlock::Create(MyContext, "else", function) : nullptr;
    BasicBlock *mergeBB = BasicBlock::Create(MyContext, "ifcont", function);

    // Generate code for the condition
    Value *condValue = condition.codeGen(context);
    IRBuilder<> builder(context.currentBlock());
    if (elseBB) {
        builder.CreateCondBr(condValue, thenBB, elseBB);
    } else {
        builder.CreateCondBr(condValue, thenBB, mergeBB);
    }

    // Generate code for the then block
    context.pushBlock(thenBB);
    trueBlock.codeGen(context);
    builder.SetInsertPoint(context.currentBlock());
    builder.CreateBr(mergeBB);
    context.popBlock();

    // Generate code for the else block (if it exists)
    if (elseBB) {
        context.pushBlock(elseBB);
        falseBlock->codeGen(context);
        builder.SetInsertPoint(context.currentBlock());
        builder.CreateBr(mergeBB);
        context.popBlock();
    }

    // Set the insertion point to the merge block
    context.pushBlock(mergeBB);
    builder.SetInsertPoint(mergeBB);

    return nullptr;
}