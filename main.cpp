#include <iostream>
#include "codegen.h"
#include "node.h"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;

void open_file(const char* filename) {
    // 打开文件
    freopen(filename, "r", stdin);
}

void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv)
{
    if (argc > 1) {
        open_file(argv[1]);
    }
    yyparse();
	cout << programBlock << endl;

	// by：wh
	// 提醒：对于新定义的 AST 节点，会将其打印为“$”
    cout << "将语法树还原为源文件如下:\n";
    if(programBlock) {
        programBlock->print(); // 调用 print 方法打印 AST
        cout << endl;
    } else {
        cout << "解析失败，无法还原为源文件。\n";
    }

    // 初始化 LLVM 相关
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    CodeGenContext context;
    createCoreFunctions(context);
    context.generateCode(*programBlock);
    context.runCode();
    
    return 0;
}
