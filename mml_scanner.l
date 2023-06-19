%option c++ prefix="mml_scanner_" outfile="mml_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "mml_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError
%}

INT       [[:digit:]]+

IDENTIFIER   [A-Za-z]([A-Za-z0-9])*

%x X_STRING X_COMMENT X_BACKSLASH X_IGNORE_STRING X_EOS_OR_OCTAL
%%
  yydebug=1; set_debug(1);

  /*  1 - Tipos de dados  */

"auto"                 return tTYPE_AUTO;
"int"                  return tTYPE_INT;
"double"               return tTYPE_DOUBLE;
"string"               return tTYPE_STRING;
"void"                 return tTYPE_VOID;

"null"                 return tNULLPTR;

"sizeof"               return tSIZEOF;

  /*  4.5 - Símbolos globais  */

"public"               return tPUBLIC;
"forward"              return tFORWARD;
"foreign"              return tFOREIGN;

  /*  4.5 - Símbolos */

"->"                   return tARROW;

  /* 3.5 - Operadores de expressões */
  /* 3.6 - Delimitadores e terminadores */

[\[\](),?{};]          return *yytext;
[-+*/%~<>@?=]          return *yytext;
">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE;
"&&"                   return tAND; 
"||"                   return tOR;

  /*  5.4 - Função principal  */

"begin"                return tBEGIN; 
"end"                  return tEND; 

  /*  6.2 - Instrução de condição  */

"if"                   return tIF;
"else"                 return tELSE;
"elif"                 return tELIF;

  /*  6.3 - Instrução de iteração  */

"while"                return tWHILE;

  /*  6.4 - Instrução de terminação  */

"stop"                 return tSTOP;

  /*  6.5 - Instrução de continuação  */

"next"                 return tNEXT;

  /*  6.8 - Instrução de impressão  */

"!"                    return tPRINT; 
"!!"                   return tPRINTLN;

  /*  7.1.2 - Leitura  */

"input"                return tINPUT; 

  /*  6.6 - Instrução de retorno  */
  
"return"               return tRETURN;

  /*  3.7 - Identificadores (nomes)  */

{IDENTIFIER}           yylval.s = new std::string(yytext); return tID;

  /*  3.2 - Comentários  */

"//".*$                /* match the whole line (it is ignored) */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|"\n"      ;  /* ignore inside comments */
  
  /*  3.8 - Literais  */
  /*  3.8.1 - Inteiros  */

[0][0-7]+                       { 
                                  try {
                                    yylval.i = std::stoi(yytext, NULL, 8);
                                    return tINTEGER;
                                  } catch (std::out_of_range &e) { yyerror("integer overflow"); }
                                }
[0]{INT}                        yyerror("invalid number (base 8 only)");

[0]|[1-9][0-9]*                 { 
                                  try {
                                    yylval.i = std::stoi(yytext, NULL, 10);
                                    return tINTEGER;
                                  } catch (std::out_of_range &e) { yyerror("integer overflow"); }
                                }

  /*  3.8.2 - Reais em vírgula flutuante  */

{INT}\.[0-9]*([Ee][-+]?{INT})?  { 
                                  try {
                                    yylval.d = std::stod(yytext, NULL);
                                    return tDOUBLE; 
                                  } catch (std::out_of_range &e) { yyerror("double overflow"); }
                                }

[0-9]*\.{INT}([Ee][-+]?{INT})?  { 
                                  try {
                                    yylval.d = std::stod(yytext, NULL);
                                    return tDOUBLE; 
                                  } catch (std::out_of_range &e) { yyerror("double overflow"); }
                                }

{INT}([Ee][-+]?{INT})           { 
                                  try {
                                    yylval.d = std::stod(yytext, NULL);
                                    return tDOUBLE; 
                                  } catch (std::out_of_range &e) { yyerror("double overflow"); }
                                }

  /*  3.8.3 - Cadeias de caracteres  */

\"                              yy_push_state(X_STRING);
<X_STRING>\\                    yy_push_state(X_BACKSLASH);
<X_STRING>\"                    {
                                  yylval.s = new std::string(strlit.str());
                                  strlit.str("");
                                  yy_pop_state();
                                  return tSTRING;
                                }
<X_STRING>\n                    yyerror("newline in string");
<X_STRING>\0                    yyerror("null in string");
<X_STRING>.                     strlit << *yytext;

<X_BACKSLASH>n                  strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                  strlit << '\r'; yy_pop_state();
<X_BACKSLASH>t                  strlit << '\t'; yy_pop_state();
<X_BACKSLASH>\"                 strlit << '"' ; yy_pop_state();
<X_BACKSLASH>\\                 strlit << '\\'; yy_pop_state();

<X_BACKSLASH>[0-7]{1,3}         {
                                  try {
                                    if ((long) strtoul(yytext, NULL, 8) > 255) yyerror("invalid integer (8 bit only)");
                                    strlit << (char)(unsigned char) strtoul(yytext, NULL, 8);
                                    yy_pop_state();
                                  } catch (std::out_of_range &e) { yyerror("integer overflow"); }
                                }
<X_BACKSLASH>.                  strlit << *yytext; yy_pop_state();

  /*  outros caracteres  */

[ \t\n\r]+                      ; /* ignore whitespace */
.                               yyerror("Unknown character");

%%
