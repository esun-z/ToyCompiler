# my_toy_compiler

Source code for "My Toy Compiler". Read about how I did on my blog:

http://gnuu.org/2009/09/18/writing-your-own-toy-compiler


## llvm 15 compatibility

## run 
./parser example.txt

## debug

lldb ./parser
breakpoint set --file codegen.cpp --line 80
run example.txt

### 可视化`AST`树结构

安装依赖：

```bash
sudo apt-get install graphviz
```

终端执行命令：

```
dot -Tpng ast.dot -o ast.png
```

> 这句命令也写入了makefile，也可以直接用`make test`，会自动生成图片

会在当前目录下生成`ast.png`，其为可视化的树结构的图片

> 提示：
>
> - 对于一般的源代码，树结构会很宽，观感较差
> - 可以使用根目录下的`simple_example.sy`，树结构会简单很多，比较直观
