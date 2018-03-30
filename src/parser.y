%code requires{
  #include <cassert>

  #include "ast.hpp"

  extern Node *g_root;

  int yylex(void);
  void yyerror(const char *);
}

%union
{
      int integer;
      double mydouble;
      std::string *mystring;
      Node *node;
      Scope *scope;
      List *arg_list;
      ParameterList *parameterlist;
      Parameter *parameter;
      Statement *statement;
      StatementList *statement_list;
      ReturnItems *ret_items;
      NumberList *NumList;
}

%token LBRACKET RBRACKET RCBRACKET LCBRACKET DOUBLENUM EQUALS COMMA FLOATTYPE ADDOP SUBOP MULTOP DIVOP LESSTHAN GREATERTHAN EQUALSEQUALS LESSTHANOREQUAL GREATERTHANOREQUAL NOTEQUAL VOID
%token VARIABLE SEMICOLON RETURN INTNUM INTTYPE DOUBLETYPE IF ELSE AND OR WHILE PLUSPLUS PLUSEQUALS MINUSMINUS MINUSEQUALS MULTEQUALS DIVEQUALS FOR MODULO DO AMPERSAND ATOF STRING_LITERAL LSBRACKET RSBRACKET


%type <node> ROOT PROGRAM FUNCTION_DECLARATION NUMBER EXPRESSION ARITHMETIC_EXPRESSION COMPARISON_EXPRESSION TERM FACTOR LIST_OF_COMPARISONS FOR COMP_LIST
%type <scope> SCOPE BODY
%type <statement> STATEMENT ARRAY JUMP_STATEMENT DECLARATION_STATEMENT SELECTION_STATEMENT FUNCTION_CALL ASSIGNMENT_STATEMENT WHILELOOP UNARY_EXPRESSION FORLOOP
%type <parameter> PARAMETER
%type <parameterlist> PARAMETER_LIST
%type <statement_list> STAT_LIST
%type <arg_list> LIST
%type <ret_items> RETURN_ITEMS
%type <NumList> NUMBERLIST

%type <integer> INTNUM
%type <mydouble> DOUBLENUM
%type <mystring> VARIABLE FLOATTYPE DOUBLETYPE VOID INTTYPE TYPE NUMTYPE IF ELSE COMP_OP LESSTHAN GREATERTHAN EQUALSEQUALS LESSTHANOREQUAL GREATERTHANOREQUAL NOTEQUAL AND OR WHILE PLUSPLUS PLUSEQUALS MINUSMINUS MINUSEQUALS MULTEQUALS DIVEQUALS MODULO AMPERSAND STRING_LITERAL


%start ROOT

%%

ROOT
      : PROGRAM { g_root = $1; }

PROGRAM
      : FUNCTION_DECLARATION { $$ = new Program($1); }
      | PROGRAM FUNCTION_DECLARATION { $$ = new Program($1, $2); }
      | DECLARATION_STATEMENT { $$ = new Program($1); }
      | PROGRAM DECLARATION_STATEMENT { $$ = new Program($1, $2); }

FUNCTION_DECLARATION
      : TYPE VARIABLE LBRACKET RBRACKET SEMICOLON { $$ = new FunctionDeclaration(*$1, *$2, NULL, NULL); }
      | TYPE VARIABLE LBRACKET RBRACKET SCOPE { $$ = new FunctionDeclaration(*$1, *$2, NULL, $5); }
      | TYPE VARIABLE LBRACKET PARAMETER_LIST RBRACKET SCOPE { $$ = new FunctionDeclaration(*$1, *$2, $4, $6); }

PARAMETER_LIST
      : PARAMETER { $$ = new ParameterList(); $$->addParameter($1); }
      | PARAMETER_LIST COMMA PARAMETER { $$ = $1; $$->addParameter($3); }

PARAMETER
      : TYPE VARIABLE { $$ = new Parameter(*$1, *$2); }

SCOPE
      : LCBRACKET RCBRACKET { $$ = new Scope(); }
      | LCBRACKET BODY RCBRACKET { $$ = $2; }

BODY
      : STAT_LIST { $$ = new Scope(); $$->addScope($1); }

STAT_LIST
      : STATEMENT { $$ = new StatementList(); $$->addStatement($1); }
      | STAT_LIST STATEMENT { $$ = $1; $$->addStatement($2); }

STATEMENT
      : JUMP_STATEMENT { $$ = $1; }
      | DECLARATION_STATEMENT { $$ = $1; }
      | SELECTION_STATEMENT { $$ = $1; }
      | FUNCTION_CALL SEMICOLON { $$ = $1; }
      | ASSIGNMENT_STATEMENT { $$ = $1; }
      | WHILELOOP { $$ = $1; }
      | UNARY_EXPRESSION SEMICOLON { $$ = $1; }
      | FORLOOP { $$ = $1; }
      | ARRAY { $$ = $1; }

WHILELOOP
      : WHILE LBRACKET COMPARISON_EXPRESSION RBRACKET SCOPE { $$ = new WhileBlock($3, $5); }
      | DO SCOPE WHILE LBRACKET COMPARISON_EXPRESSION RBRACKET SEMICOLON { $$ = new WhileBlock($5, $2, true);}

FORLOOP
      : FOR LBRACKET DECLARATION_STATEMENT COMPARISON_EXPRESSION SEMICOLON EXPRESSION RBRACKET SCOPE { $$ = new ForLoop($3, $4, $6, $8); }
      | FOR LBRACKET ASSIGNMENT_STATEMENT COMPARISON_EXPRESSION SEMICOLON EXPRESSION RBRACKET SCOPE { $$ = new ForLoop($3, $4, $6, $8); }
      | FOR LBRACKET SEMICOLON COMPARISON_EXPRESSION SEMICOLON EXPRESSION RBRACKET SCOPE { $$ = new ForLoop(NULL, $4, $6, $8); }

ARRAY
      : TYPE VARIABLE LSBRACKET NUMBER RSBRACKET SEMICOLON { $$ = new Array(*$1, *$2, $4, NULL, NULL); }
      | VARIABLE LSBRACKET NUMBER RSBRACKET EQUALS NUMBER SEMICOLON { $$ = new Array("non", *$1, $3, $6, NULL);  }
      | TYPE VARIABLE LSBRACKET NUMBER RSBRACKET EQUALS LCBRACKET NUMBERLIST RCBRACKET SEMICOLON { $$ = new Array(*$1, *$2, $4, NULL, $8); }
      | VARIABLE LSBRACKET NUMBER RSBRACKET { $$ = new Array("non",*$1, NULL, $3, NULL); }

JUMP_STATEMENT
      : RETURN RETURN_ITEMS SEMICOLON { $$ = new Return($2); }
      | RETURN LBRACKET RETURN_ITEMS RBRACKET SEMICOLON { $$ = new Return($3); }

DECLARATION_STATEMENT
      : TYPE VARIABLE SEMICOLON { $$ = new Declaration(*$1, *$2); }
      | TYPE VARIABLE EQUALS ARITHMETIC_EXPRESSION SEMICOLON { $$ = new Declaration(*$1, *$2, $4); }
      | TYPE MULTOP VARIABLE SEMICOLON { $$ = new Declaration(*$1, *$3, NULL, true); }
      | TYPE MULTOP VARIABLE EQUALS ARITHMETIC_EXPRESSION SEMICOLON { $$ = new Declaration(*$1, *$3, $5, true); }

ASSIGNMENT_STATEMENT
      : VARIABLE EQUALS ARITHMETIC_EXPRESSION SEMICOLON { $$ = new Declaration ("non", *$1, $3); }
      | MULTOP VARIABLE EQUALS ARITHMETIC_EXPRESSION SEMICOLON { $$ = new Declaration("non", *$2, $4, true); }

SELECTION_STATEMENT
      : IF LBRACKET LIST_OF_COMPARISONS RBRACKET SCOPE { $$ = new IfBlock($3 , $5, NULL); }
      | IF LBRACKET LIST_OF_COMPARISONS RBRACKET SCOPE ELSE SCOPE { $$ = new IfBlock($3, $5, $7); }

EXPRESSION
      : ARITHMETIC_EXPRESSION { $$ = $1; }
      | COMPARISON_EXPRESSION { $$ = $1; }

UNARY_EXPRESSION
      : FACTOR PLUSPLUS { $$ = new UnaryExpression($1, *$2, new IntNum(1)); }
      | FACTOR PLUSEQUALS TERM { $$ = new UnaryExpression($1, *$2, $3); }
      | FACTOR MINUSMINUS { $$ = new UnaryExpression($1, *$2, new IntNum(1)); }
      | FACTOR MINUSEQUALS TERM { $$ = new UnaryExpression($1, *$2, $3); }
      | FACTOR MULTEQUALS TERM { $$ = new UnaryExpression($1, *$2, $3); }
      | FACTOR DIVEQUALS TERM { $$ = new UnaryExpression($1, *$2, $3); }
      | PLUSPLUS FACTOR { $$ = new UnaryExpression($2, *$1, new IntNum(1), true);}
      | MINUSMINUS FACTOR { $$ = new UnaryExpression($2, *$1, new IntNum(1), true);}

ARITHMETIC_EXPRESSION
      : TERM { $$ = $1; }
      | ARITHMETIC_EXPRESSION ADDOP TERM { $$ = new BinaryExpression($1, "+", $3); }
      | ARITHMETIC_EXPRESSION SUBOP TERM  { $$ = new BinaryExpression($1, "-", $3); }

TERM
      : FACTOR { $$ = $1; }
      | TERM MULTOP FACTOR { $$ = new BinaryExpression($1, "*", $3); }
      | TERM DIVOP FACTOR { $$ = new BinaryExpression($1, "/", $3); }
      | TERM MODULO FACTOR { $$ = new BinaryExpression($1, "%", $3); }

FACTOR
      : NUMBER { $$ = $1; }
      | FUNCTION_CALL { $$ = $1; }
      | LBRACKET EXPRESSION RBRACKET { $$ = $2; }
      | VARIABLE { $$ = new Variable(*$1); }
      | ARRAY { $$ = $1; }
      | UNARY_EXPRESSION { $$ = $1; }
      | MULTOP VARIABLE { $$ = new Pointer(*$2); }
      | AMPERSAND VARIABLE { $$ = new Reference(*$2); }
      | ATOF LBRACKET STRING_LITERAL RBRACKET { $$ = new AtoF(*$3); }

COMPARISON_EXPRESSION
      : EXPRESSION COMP_OP EXPRESSION { $$ = new BinaryExpression($1, *$2, $3); }
      | LBRACKET COMPARISON_EXPRESSION RBRACKET { $$ = $2; }

FUNCTION_CALL
      : VARIABLE LBRACKET LIST RBRACKET { $$ = new FunctionCall(*$1, $3); }
      | VARIABLE LBRACKET RBRACKET { $$ = new FunctionCall(*$1); }

LIST
      : ARITHMETIC_EXPRESSION { $$ = new List(); $$->addList($1); }
      | LIST COMMA ARITHMETIC_EXPRESSION { $$->addList($3); $$ = $1; }

RETURN_ITEMS
      : ARITHMETIC_EXPRESSION { $$ = new ReturnItems(); $$->addRet(new ReturnExp($1)); }

LIST_OF_COMPARISONS
      : LBRACKET LIST_OF_COMPARISONS RBRACKET { $$ = $2; }
      | COMP_LIST { $$ = $1; }
      | LIST_OF_COMPARISONS OR COMP_LIST { $$ = new BinaryExpression($1, "or", $3); }

COMP_LIST
      : EXPRESSION { $$ = $1; }
      | LIST_OF_COMPARISONS AND COMP_LIST { $$ = new BinaryExpression($1, "and", $3); }


////////////////////////////////////////////////////////////////////////////
NUMBERLIST
      : NUMBER { $$ = new NumberList(); $$->addNumber($1); }
      | NUMBERLIST COMMA NUMBER { $$ = $1; $$->addNumber($3); }

NUMBER
      : INTNUM { $$ = new IntNum($1); }
      | SUBOP INTNUM { $$ = new IntNum($2*-1); }
      | DOUBLENUM { $$ = new DoubleNum($1); }

TYPE
      : VOID { $$ = new std::string("void"); }
      | NUMTYPE { $$ = $1; }

NUMTYPE
      : INTTYPE { $$ = new std::string("int"); }
      | DOUBLETYPE { $$ = new std::string("double"); }
      | FLOATTYPE { $$ = new std::string("float"); }

COMP_OP
      : LESSTHAN { $$ = new std::string("<"); }
      | GREATERTHAN { $$ = new std::string(">"); }
      | EQUALSEQUALS { $$ = new std::string("=="); }
      | LESSTHANOREQUAL { $$ = new std::string("<="); }
      | GREATERTHANOREQUAL { $$ = new std::string(">="); }
      | NOTEQUAL { $$ = new std::string("!="); }


%%

Node *g_root;

const Node *parseAST()
{
  g_root = 0;
  yyparse();
  return g_root;
}
