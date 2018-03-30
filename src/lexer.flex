%option noyywrap

%{
/* Now in a section of C that will be embedded
   into the auto-generated code. Flex will not
   try to interpret code surrounded by %{ ... %} */

/* Bring in our declarations for token types and
   the yylval variable. */
#include <cstdio>
#include <iostream>
#include <string>


// This is to work around an irritating bug in Flex
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex
extern "C" int fileno(FILE *stream);

/* End the embedded code section. */
#include "parser.tab.hpp"
%}


%%

"return"		{ return RETURN; }

"int" 			{ yylval.mystring = new std::string(yytext); return INTTYPE; }
"double"        { yylval.mystring = new std::string(yytext); return DOUBLETYPE; }
"float"         { yylval.mystring = new std::string(yytext); return FLOATTYPE; }
"void"          { yylval.mystring = new std::string(yytext); return VOID; }

"if"            { yylval.mystring = new std::string(yytext); return IF; }
"else"          { yylval.mystring = new std::string(yytext); return ELSE; }
"while"         { yylval.mystring = new std::string(yytext); return WHILE; }
"for"           { yylval.mystring = new std::string(yytext); return FOR; }
"do"            { yylval.mystring = new std::string(yytext); return DO; }
"atof"          { yylval.mystring = new std::string(yytext); return ATOF; }

"+"             {  yylval.mystring = new std::string(yytext); return ADDOP; }
"-"             { yylval.mystring = new std::string(yytext); return SUBOP; }
"*"             { yylval.mystring = new std::string(yytext); return MULTOP; }
"/"             { yylval.mystring = new std::string(yytext); return DIVOP; }
"%"             { yylval.mystring = new std::string(yytext); return MODULO;}
"<"             { yylval.mystring = new std::string(yytext); return LESSTHAN; }
">"             { yylval.mystring = new std::string(yytext); return GREATERTHAN; }
"=="            { yylval.mystring = new std::string(yytext); return EQUALSEQUALS; }
"<="            { yylval.mystring = new std::string(yytext); return LESSTHANOREQUAL; }
">="            { yylval.mystring = new std::string(yytext); return GREATERTHANOREQUAL; }
"!="            { yylval.mystring = new std::string(yytext); return NOTEQUAL; }
"&&"            { yylval.mystring = new std::string(yytext); return AND; }
"||"            { yylval.mystring = new std::string(yytext); return OR; }
"&"             { yylval.mystring = new std::string(yytext); return AMPERSAND; }

"++"            { yylval.mystring = new std::string(yytext); return PLUSPLUS; }
"+="            { yylval.mystring = new std::string(yytext); return PLUSEQUALS; }
"--"            { yylval.mystring = new std::string(yytext); return MINUSMINUS; }
"-="            { yylval.mystring = new std::string(yytext); return MINUSEQUALS; }
"*="            { yylval.mystring = new std::string(yytext); return MULTEQUALS; }
"/="            { yylval.mystring = new std::string(yytext); return DIVEQUALS; }


[(]			{ return LBRACKET; }
[)]			{ return RBRACKET; }
[{]			{ return LCBRACKET; }
[}]			{ return RCBRACKET; }
"["     { return LSBRACKET; }
"]"     { return RSBRACKET; }
[;]			{ return SEMICOLON; }
[=]         { return EQUALS; }
[,]         { return COMMA; }


[a-zA-Z_]+		{ yylval.mystring = new std::string(yytext); return VARIABLE; }
[0-9]+		{ yylval.integer = std::stoi(yytext, NULL); return INTNUM; }
\"(\\.|[^"\\])*\" { yylval.mystring = new std::string(yytext); return STRING_LITERAL; }
-?[0-9]+"."([0-9]*)? { yylval.mydouble = std::stod(yytext, NULL); return DOUBLENUM; }

"//".*          /* Ignore comments */
"/*".*"*/"      /* Ignore comments */

[ \t\n]         /* Ignore whitespace */

%%

void yyerror(char const *s)
{
    exit(1);
}
