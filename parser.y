%{
	#include "node.h"
    #include <cstdio>
    #include <cstdlib>
	#define PRINT_PROD(name) fprintf(stderr, "%s\n", #name)

	NBlock *programBlock; /* the top level root node of our final AST */

	extern int yylex();
	void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
	Node *node;
	NBlock *block;
	NExpression *expr;
	NIfStatement *ifstmt;
	NWhileStatement *whilestmt;
	NStatement *stmt;
	NIdentifier *ident;
	NVariableDeclaration *var_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::string *string;
	int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%debug
%token <string> TIDENTIFIER TINTEGER TFLOAT
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACKET TRBRACKET TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON
%token <token> TPLUS TMINUS TMUL TDIV TMOD TNOT
%token <token> TRETURN TCONST TIF TELSE TWHILE TBREAK TCONTINUE
%token <token> TOR TAND

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric expr 
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl const_decl ifstmt whilestmt
%type <token> comparison

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV



%start program

%%

program : stmts { programBlock = $1; }
		;
		
stmts : stmt { PRINT_PROD(stmts->stmt); $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
	  | block { PRINT_PROD(stmts->block); $$ = $1; }
	  | ifstmt { PRINT_PROD(stmts->ifstmt); $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
	  | whilestmt { PRINT_PROD(stmts->whilestmt); $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
	  | stmts stmt { PRINT_PROD(stmts->stmts stmt); $1->statements.push_back($<stmt>2); }
	  | stmts block { PRINT_PROD(stmts->stmts block); $$->blocks.push_back($<block>2); }
	  | stmts ifstmt { PRINT_PROD(stmts->stmts ifstmt); $$->statements.push_back($<stmt>2); }
	  | stmts whilestmt { PRINT_PROD(stmts->stmts whilestmt); $$->statements.push_back($<stmt>2); }
	  ;

ifstmt	: TIF TLPAREN expr TRPAREN block { PRINT_PROD(ifstmt->if(expr)block); $$ = new NIfStatement(*$3, *$5); }
		| TIF TLPAREN expr TRPAREN stmt { PRINT_PROD(ifstmt->if(expr)stmt); $$ = new NIfStatement(*$3, *(new NBlock(*$5))); }
		| TIF TLPAREN expr TRPAREN block TELSE block { PRINT_PROD(ifstmt->if lp expr rp block else block); $$ = new NIfStatement(*$3, *$5, *$7); }
		| TIF TLPAREN expr TRPAREN stmt TELSE block { PRINT_PROD(ifstmt->if lp expr rpstmt else block); $$ = new NIfStatement(*$3, *(new NBlock(*$5)), *$7); }
		| TIF TLPAREN expr TRPAREN block TELSE stmt { PRINT_PROD(ifstmt->if lp expr rp block else stmt); $$ = new NIfStatement(*$3, *$5, *(new NBlock(*$7))); }
		| TIF TLPAREN expr TRPAREN stmt TELSE stmt { PRINT_PROD(ifstmt->if lp expr rp stmt else stmt); $$ = new NIfStatement(*$3, *(new NBlock(*$5)), *(new NBlock(*$7))); }
		;

whilestmt	: TWHILE TLPAREN expr TRPAREN block { PRINT_PROD(whilestmt->while lp expr rp block); $$ = new NWhileStatement(*$3, *$5); }
			| TWHILE TLPAREN expr TRPAREN stmt { PRINT_PROD(whilestmt->while lp expr rp stmt); $$ = new NWhileStatement(*$3, *(new NBlock(*$5))); }
			;

stmt : var_decl TSEMICOLON {PRINT_PROD(stmt->var_decl semi);}
	 | func_decl {PRINT_PROD(stmt->func_decl);}
	 | const_decl TSEMICOLON {PRINT_PROD(stmt->const_decl semi);}
	 | expr TSEMICOLON { PRINT_PROD(stmt->expr semi); $$ = new NExpressionStatement(*$1); }
	 | TRETURN expr TSEMICOLON { PRINT_PROD(stmt->return expr semi); $$ = new NReturnStatement(*$2); }
     ;

block : TLBRACE stmts TRBRACE { PRINT_PROD(block->lbrace stmts rbrace); $$ = $2; }
	  | TLBRACE TRBRACE { PRINT_PROD(block->lbrace rbrace); $$ = new NBlock(); }
	  ;

var_decl : ident ident { PRINT_PROD(var_decl->type ident); $$ = new NVariableDeclaration(*$1, *$2); }
		 | ident ident TEQUAL expr { PRINT_PROD(var_decl->type ident eq expr); $$ = new NVariableDeclaration(*$1, *$2, $4); }
		 ;

const_decl	: TCONST ident ident { PRINT_PROD(const_decl->const type ident); $$ = new NVariableDeclaration(*$2, *$3); }
			| TCONST ident ident TEQUAL expr { PRINT_PROD(const_decl->const type ident eq expr); $$ = new NVariableDeclaration(*$2, *$3, $5); }
            ;

func_decl : ident ident TLPAREN func_decl_args TRPAREN block 
			{ PRINT_PROD(func_decl->type ident lp args rp block); $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
		  ;
	
func_decl_args : /*blank*/  { PRINT_PROD(args->blank); $$ = new VariableList(); }
		  | var_decl { PRINT_PROD(args->var_decl); $$ = new VariableList(); $$->push_back($<var_decl>1); }
		  | func_decl_args TCOMMA var_decl { PRINT_PROD(args->args comma var_decl); $1->push_back($<var_decl>3); }
		  ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
	  ;

numeric : TINTEGER { PRINT_PROD(numeric->intConst); $$ = new NInteger(atol($1->c_str())); delete $1; }
		| TFLOAT {	PRINT_PROD(numeric->floatConst); $$ = new NDouble(atof($1->c_str())); delete $1; }
		;
	
expr : ident TEQUAL expr { PRINT_PROD(expr->ident eq expr); $$ = new NAssignment(*$<ident>1, *$3); }
	 | ident TLPAREN call_args TRPAREN { PRINT_PROD(expr->ident lp call_args rp); $$ = new NMethodCall(*$1, *$3); delete $3; }
	 | ident {  PRINT_PROD(expr->ident); $<ident>$ = $1; }
	 | numeric { PRINT_PROD(expr->numeric);}
     | expr TMUL expr { PRINT_PROD(expr->expr mul expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TDIV expr { PRINT_PROD(expr->expr div expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
	 | expr TMOD expr { PRINT_PROD(expr->expr mod expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TPLUS expr { PRINT_PROD(expr->expr plus expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TMINUS expr { PRINT_PROD(expr->expr minus expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
 	 | expr comparison expr { PRINT_PROD(expr->expr comparison expr); $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { PRINT_PROD(expr->lp expr rp); $$ = $2; }
	 ;
	
call_args : /*blank*/  { PRINT_PROD(call_args->blank); $$ = new ExpressionList(); }
		  | expr { PRINT_PROD(call_args->expr); $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args TCOMMA expr  { PRINT_PROD(call_args->call_args comma expr); $1->push_back($3); }
		  ;

comparison	: TCEQ {PRINT_PROD(comparison->ceq);}
			| TCNE {PRINT_PROD(comparison->cne);}
			| TCLT {PRINT_PROD(comparison->clt);}
			| TCLE {PRINT_PROD(comparison->cle);}
			| TCGT {PRINT_PROD(comparison->cgt);}
			| TCGE {PRINT_PROD(comparison->cge);}
			;

%%
