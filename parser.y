%{
	#include "node.h"
    #include <cstdio>
    #include <cstdlib>
	#define PRINT_PROD(name) fprintf(stderr, "%s\n", #name)

	NCompUnit *programCompUnit; /* the top level root node of our final AST */

	extern int yylex();
	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
	Node *node;
	NCompUnit *comp_unit;
	NBlock *block;
	NExpr *expr;
	NStmt *stmt;
	NIdent *ident;
	NFuncDecl *func_decl;
	NVarDecl *var_decl;
	std::vector<NVarDecl*> *varvec;
	std::vector<NExpr*> *exprvec;
	std::string *string;
	long long number_int;
	double number_float;
	int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%debug
%token <number_int> TINTEGER
%token <number_float> TFLOAT
%token <string> TIDENTIFIER 
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACKET TRBRACKET TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON
%token <token> TPLUS TMINUS TMUL TDIV TMOD TNOT
%token <token> TRETURN TCONST TIF TELSE TWHILE TBREAK TCONTINUE
%token <token> TOR TAND
%token <token> TINTTYPE TFLOATTYPE TVOIDTYPE

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <comp_unit> comp_unit
%type <ident> ident
%type <expr> numeric expr
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> stmts block
%type <var_decl> var_decl
%type <func_decl> func_decl
%type <stmt> stmt ifstmt whilestmt

/* Solve IF-ELSE conflict */
%nonassoc IFX
%nonassoc TELSE

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV TMOD
%nonassoc TNOT
%left TAND
%left TOR
%nonassoc TCEQ TCNE TCLT TCLE TCGT TCGE

%left TEQUAL

%start program

%%

program	: comp_unit { programCompUnit = $1; }
		;
		
comp_unit	: var_decl TSEMICOLON { $$ = new NCompUnit(); $$->decls.push_back($1);}
	  		| func_decl { $$ = new NCompUnit(); $$->decls.push_back($1); }
	  		| comp_unit var_decl TSEMICOLON { $1->decls.push_back($2); }
	  		| comp_unit func_decl { $1->decls.push_back($2); }
	  		;

var_decl	: TCONST TINTTYPE ident TEQUAL expr { $3->type = $2; $$ = new NVarDecl(true, *$3, $5); }
			| TCONST TFLOATTYPE ident TEQUAL expr { $3->type = $2; $$ = new NVarDecl(true, *$3, $5); }
			| TINTTYPE ident { $2->type = $1; $$ = new NVarDecl(false, *$2); }
			| TFLOATTYPE ident { $2->type = $1; $$ = new NVarDecl(false, *$2); }
			| TINTTYPE ident TEQUAL expr { $2->type = $1; $$ = new NVarDecl(false, *$2, $4); }
			| TFLOATTYPE ident TEQUAL expr { $2->type = $1; $$ = new NVarDecl(false, *$2, $4); }
			;

func_decl : TVOIDTYPE ident TLPAREN func_decl_args TRPAREN block { $2->type = $1; $$ = new NFuncDecl(*$2, *$4, *$6); delete $4; }
		  | TINTTYPE ident TLPAREN func_decl_args TRPAREN block { $2->type = $1; $$ = new NFuncDecl(*$2, *$4, *$6); delete $4; }
		  | TFLOATTYPE ident TLPAREN func_decl_args TRPAREN block { $2->type = $1; $$ = new NFuncDecl(*$2, *$4, *$6); delete $4; }
		  ;

func_decl_args : /*blank*/  { $$ = new VariableList(); }
		  | var_decl { $$ = new VariableList(); $$->push_back($1); }
		  | func_decl_args TCOMMA var_decl { $1->push_back($3); }
		  ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
	  | TLBRACE TRBRACE { $$ = new NBlock(); }
	  ;

stmts : stmt { $$ = new NBlock(); $$->statements.push_back($1); }
	  | block { $$ = $1; }
	  | stmts stmt { $1->statements.push_back($2); }
	  | stmts block { $$->statements.push_back($2); }
	  ;

stmt : var_decl TSEMICOLON { $$ = $1; }
	 | func_decl { $$ = $1; }
	 | expr TSEMICOLON { $$ = new NExprStmt(*$1); }
	 | TRETURN expr TSEMICOLON { $$ = new NReturnStmt(*$2); }
	 | ifstmt { $$ = $1; }
	 | whilestmt { $$ = $1; }
     ;

ifstmt	: TIF TLPAREN expr TRPAREN block %prec IFX { $$ = new NIfStmt(*$3, *$5); }
		| TIF TLPAREN expr TRPAREN stmt %prec IFX { $$ = new NIfStmt(*$3, *(new NBlock(*$5))); }
		| TIF TLPAREN expr TRPAREN block TELSE block { $$ = new NIfStmt(*$3, *$5, *$7); }
		| TIF TLPAREN expr TRPAREN stmt TELSE block { $$ = new NIfStmt(*$3, *(new NBlock(*$5)), *$7); }
		| TIF TLPAREN expr TRPAREN block TELSE stmt { $$ = new NIfStmt(*$3, *$5, *(new NBlock(*$7))); }
		| TIF TLPAREN expr TRPAREN stmt TELSE stmt { $$ = new NIfStmt(*$3, *(new NBlock(*$5)), *(new NBlock(*$7))); }
		;

whilestmt	: TWHILE TLPAREN expr TRPAREN block { $$ = new NWhileStmt(*$3, *$5); }
			| TWHILE TLPAREN expr TRPAREN stmt { $$ = new NWhileStmt(*$3, *(new NBlock(*$5))); }
			;

ident : TIDENTIFIER { $$ = new NIdent(*$1); delete $1; }
	  ;

numeric : TINTEGER { $$ = new NInteger($1); }
		| TFLOAT {	$$ = new NFloat($1); }
		;
	
expr : ident TEQUAL expr { $$ = new NAssignment(*$1, *$3); }
	 | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
	 | ident { $$ = $1; }
	 | numeric { $$ = $1; }
     | expr TMUL expr { $$ = new NBinaryExpr(*$1, $2, *$3); }
     | expr TDIV expr { $$ = new NBinaryExpr(*$1, $2, *$3); }
	 | expr TMOD expr { $$ = new NBinaryExpr(*$1, $2, *$3); }
     | expr TPLUS expr { $$ = new NBinaryExpr(*$1, $2, *$3); }
     | expr TMINUS expr { $$ = new NBinaryExpr(*$1, $2, *$3); }
 	 | expr TCEQ expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TCNE expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TCLT expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TCLE expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TCGT expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TCGE expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TAND expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | expr TOR expr { $$ = new NLogicalBinaryExpr(*$1, $2, *$3); }
	 | TMINUS expr { $$ = new NUnaryExpr($1, *$2); }
	 | TNOT expr { $$ = new NLogicalUnaryExpr($1, *$2); }
     | TLPAREN expr TRPAREN { $$ = $2; }
	 ;
	
call_args : /*blank*/  { $$ = new ExprList(); }
		  | expr { $$ = new ExprList(); $$->push_back($1); }
		  | call_args TCOMMA expr  { $1->push_back($3); }
		  ;

%%
