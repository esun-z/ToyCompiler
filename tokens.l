%{
#include <string>
#include "node.h"
#include "parser.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define SAVE_INT yylval.number_int = atoll(yytext)
#define SAVE_FLOAT yylval.number_float = atof(yytext)
#define TOKEN(t)    (yylval.token = t)

void SkipSingleLineComment();
void SkipMultiLineComment();

long long lineCount = 1;

%}

%option noyywrap

%%
"//".*  { SkipSingleLineComment();}
"/*"    { SkipMultiLineComment();}
[ \t]					        ;
\n                              lineCount++;
"const"                        return TOKEN(TCONST);
"if"                            return TOKEN(TIF);
"else"                          return TOKEN(TELSE);
"while"                         return TOKEN(TWHILE);
"break"                         return TOKEN(TBREAK);
"continue"                      return TOKEN(TCONTINUE);
"return"				        return TOKEN(TRETURN);
"int"                           return TOKEN(TINTTYPE);
"float"                         return TOKEN(TFLOATTYPE);
"void"                          return TOKEN(TVOIDTYPE);
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]* 			    SAVE_FLOAT; return TFLOAT;
[0-9]+					        SAVE_INT; return TINTEGER;

"="						          return TOKEN(TEQUAL);
"=="				          	return TOKEN(TCEQ);
"!="			          		return TOKEN(TCNE);
"<"				          		return TOKEN(TCLT);
"<="	          				return TOKEN(TCLE);
">"				          		return TOKEN(TCGT);
">="					          return TOKEN(TCGE);

"("	          					return TOKEN(TLPAREN);
")"					          	return TOKEN(TRPAREN);
"["                             return TOKEN(TLBRACKET);
"]"                             return TOKEN(TRBRACKET);
"{"         						return TOKEN(TLBRACE);
"}"					          	return TOKEN(TRBRACE);

"."         						return TOKEN(TDOT);
","				          		return TOKEN(TCOMMA);
";"				          		return TOKEN(TSEMICOLON);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);
"%"				          		return TOKEN(TMOD);
"!"                             return TOKEN(TNOT);

"&&"                            return TOKEN(TAND);
"||"                            return TOKEN(TOR);

.                       printf("Unknown token!\n"); yyterminate();

%%

void SkipSingleLineComment() {
    char c;
#ifndef __cplusplus
    while ((c = input()) != '\n' && c != EOF);
#else
    while ((c = yyinput()) != '\n' && c != EOF);
#endif
    // place back EOF, or increase line count
    if(c == EOF){
        unput(c);
    }
}

void SkipMultiLineComment() {
    char c;
#ifndef __cplusplus
    while ((c = input()) != EOF) {
#else
    while ((c = yyinput()) != EOF) {
#endif
        if (c == '*') {
#ifndef __cplusplus
            c = input();
#else
            c = yyinput();
#endif
            if (c == '/') {
                return;
            } else {
                unput(c);
            }
        }
    }
    // handle unclosed comment
    if(c == EOF) {
        fprintf(stderr, "Unclosed comment, except */.\n");
    }
}