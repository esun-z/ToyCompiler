/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TIDENTIFIER = 258,             /* TIDENTIFIER  */
    TINTCONST = 259,               /* TINTCONST  */
    TFLOATCONST = 260,             /* TFLOATCONST  */
    TCONST = 261,                  /* TCONST  */
    TINT = 262,                    /* TINT  */
    TFLOAT = 263,                  /* TFLOAT  */
    TVOID = 264,                   /* TVOID  */
    TIF = 265,                     /* TIF  */
    TELSE = 266,                   /* TELSE  */
    TWHILE = 267,                  /* TWHILE  */
    TBREAK = 268,                  /* TBREAK  */
    TCONTINUE = 269,               /* TCONTINUE  */
    TCEQ = 270,                    /* TCEQ  */
    TCNE = 271,                    /* TCNE  */
    TCLT = 272,                    /* TCLT  */
    TCLE = 273,                    /* TCLE  */
    TCGT = 274,                    /* TCGT  */
    TCGE = 275,                    /* TCGE  */
    TEQUAL = 276,                  /* TEQUAL  */
    TLPAREN = 277,                 /* TLPAREN  */
    TRPAREN = 278,                 /* TRPAREN  */
    TLBRACKET = 279,               /* TLBRACKET  */
    TRBRACKET = 280,               /* TRBRACKET  */
    TLBRACE = 281,                 /* TLBRACE  */
    TRBRACE = 282,                 /* TRBRACE  */
    TCOMMA = 283,                  /* TCOMMA  */
    TDOT = 284,                    /* TDOT  */
    TPLUS = 285,                   /* TPLUS  */
    TMINUS = 286,                  /* TMINUS  */
    TMUL = 287,                    /* TMUL  */
    TDIV = 288,                    /* TDIV  */
    TMOD = 289,                    /* TMOD  */
    TNOT = 290,                    /* TNOT  */
    TRETURN = 291,                 /* TRETURN  */
    TAND = 292,                    /* TAND  */
    TOR = 293,                     /* TOR  */
    TSEMICOLON = 294,              /* TSEMICOLON  */
    LOWER_THAN_ELSE = 295,         /* LOWER_THAN_ELSE  */
    LOWER_THAN_DECL = 296,         /* LOWER_THAN_DECL  */
    DECL = 297                     /* DECL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
