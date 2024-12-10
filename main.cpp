#include <iostream>
#include "codegen.h"
#include "node.h"
#include <fstream> // 添加此行以支持文件输出

using namespace std;

extern int yyparse();
extern NCompUnit* programCompUnit;

extern int yydebug;



void open_file(const char* filename) {
	// openfile
	freopen(filename, "r", stdin);
}

void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv)
{
	yydebug = 1;
	if (argc > 1) {
		open_file(argv[1]);
	}
	yyparse();
	// 提醒：对于新定义的 AST 节点，会将其打印为“$”
    cout << "将语法树还原为源文件如下:\n";
    if(programCompUnit) {
        programCompUnit->print(); // 调用 print 方法打印 AST
        cout << endl;
    } else {
        cout << "解析失败，无法还原为源文件。\n";
    }
	cout << programCompUnit << endl;


    // 生成 AST 的 DOT 文件
    if(programCompUnit) {
        ofstream dotFile("ast.dot");
        if(!dotFile.is_open()) {
            cerr << "无法创建 ast.dot 文件。\n";
            return 1;
        }
        dotFile << "digraph AST {\n";
        int currentId = 0;
        programCompUnit->generateDot(dotFile, currentId);
        dotFile << "}\n";
        dotFile.close();
        cout << "AST 已写入 ast.dot 文件。\n";
    }
    
    // see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();
	CodeGenContext context;
	createCoreFunctions(context);
	context.generateCode(*programCompUnit);
	context.runCode();
	
	return 0;
}

