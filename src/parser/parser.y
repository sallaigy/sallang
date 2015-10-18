%{
    #include "ast.h"
    #include "compile.h"
    #include <stdio.h>
    #include <stdlib.h>

    int yylex(void);
    void yyerror(const char *s);
    extern int yylineno;

    extern char *yytext;
    extern CompilerContext context;

%}

%error-verbose

%union {
    SValue sval;
    AstNode *ast;
    char *string;
    SValueType type;
};

%destructor { AstNode_destroy($$); } <ast>

//%type <ast> inner_stmt_list inner_stmt

%right '='
%left T_LOGICAL_AND T_LOGICAL_OR
%left '<' '>' T_IS_EQ T_IS_NOT_EQ T_IS_GTEQ T_IS_LTEQ
%left '+' '-'
%left '*' '/' '%'
%left '!'


%token <sval>   T_INTEGER_LITERAL   "integer literal (T_INTEGER_LITERAL)"
%token <sval>   T_DOUBLE_LITERAL    "double literal (T_DOUBLE_LITERAL)"
%token <sval>   T_BOOLEAN_LITERAL   "boolean literal (T_BOOLEAN_LITERAL)"
%token <sval>   T_STRING_LITERAL    "string literal (T_STRING_LITERAL)"

%token <string> T_IDENTIFIER        "identifier (T_IDENTIFIER)" 
%token T_INT_TYPE T_DOUBLE_TYPE T_BOOL_TYPE

%token T_LET
%token T_IN
%token T_END
%token T_VAR
%token T_CONST
%token T_PRINT
%token T_FUNC

%token T_IF
%token T_ELSE
%token T_WHILE

%token T_IS_EQ
%token T_IS_NOT_EQ
%token T_IS_GTEQ
%token T_IS_LTEQ
%token T_LOGICAL_AND
%token T_LOGICAL_OR

%start start

%type <ast> program
%type <ast> declaration_list declaration stmt_list  stmt /*func_list func */
//%type <ast> var_decl const_decl
%type <ast> if_stmt while_stmt
%type <ast> if_stmt_without_else
%type <ast> expr
%type <type> typename
%type <ast> id
%type <ast> scalar

%%

start:
    program     {
                    context.ast_root = $1;
                }
    ;

program:
    T_LET declaration_list T_IN stmt_list T_END {
        $$ = AstNode_create(yylineno, AST_ROOT, $2, $4, NULL);
    }
    ;
/*
program:
    func_list   {
        $$ = AstNode_create(yylineno, AST_ROOT, $1, NULL, NULL);
    }
    ;
*/
declaration_list:
    declaration_list declaration                  { $$ = AstNode_list_add($1, $2); }
    |                                             { $$ = AstNode_create_list(); }
    ;

declaration:
    T_VAR id ':' typename ';'                    {
        /* Symbol_push($2, $4, NULL); */
        $$ = AstNode_create_decl(yylineno, $2, $4);
    }

typename:
    T_INT_TYPE                                    { $$ = SVALUE_INTEGER; }
    | T_DOUBLE_TYPE                               { $$ = SVALUE_DOUBLE; }
    | T_BOOL_TYPE                                 { $$ = SVALUE_BOOLEAN; }
    ;
/*
func_list:
    func_list func                              { $$ = AstNode_list_add($1, $2); }
    |                                           { $$ = AstNode_create_list(); }
    ;

func:
    T_FUNC id '(' ')' '{' stmt_list '}' { 
        $$ = AstNode_create(yylineno, AST_FUNCTION, $2, NULL, $6);
    }
*/
stmt_list:
    stmt_list stmt                              { $$ = AstNode_list_add($1, $2); }
    |                                           { $$ = AstNode_create_list(); }
    ;
/*
inner_stmt_list:
    innert_stmt_list inner_stmt                 { $$ = AstNode_list_add($1, $2); }
    |                                           { $$ = AstNode_create_list(); }
    ;

inner_stmt:
    stmt
    ;
*/
stmt:
    '{' stmt_list '}'           { $$ = $2; }
    | declaration ';'           { $$ = $1; }
    | expr ';'                  { $$ = $1; }
    | T_PRINT '(' expr ')' ';'  { $$ = AstNode_create(yylineno, AST_PRINT_STMT, $3, NULL, NULL); }
    | if_stmt                   { $$ = $1; }
    | while_stmt                { $$ = $1; }
    ;
/*
inner_stmt_list:
    inner_stmt_list inner_stmt                  { $$ = AstNode_list_add($1, $2); }
    |                                           { $$ = AstNode_create_list(); }
    ;

inner_stmt:
    stmt                                        { $$ = $1; }
    ;
*/
if_stmt:
    if_stmt_without_else {
        $$ = $1;
    }
    | T_IF '(' expr ')' stmt T_ELSE stmt {
        $$ = AstNode_create(yylineno, AST_IF, $3, $5, $7);
    }
    ;

if_stmt_without_else:
    T_IF '(' expr ')' stmt {
        $$ = AstNode_create(yylineno, AST_IF, $3, $5, NULL);
    }
    ;

while_stmt:
    T_WHILE '(' expr ')' stmt {
        $$ = AstNode_create(yylineno, AST_WHILE, $3, $5, NULL);
    }

expr:
    scalar                    { $$ = $1; }
    | id                      { $$ = $1; }
    | '!' expr                { $$ = AstNode_create(yylineno, AST_UNARY_NOT, $2, NULL, NULL); }
    | id '=' expr             { $$ = AstNode_create(yylineno, AST_BINARY_ASSIGN, $1, $3, NULL); }
    | expr '+' expr           { $$ = AstNode_create(yylineno, AST_BINARY_ADD, $1, $3, NULL); }
    | expr '-' expr           { $$ = AstNode_create(yylineno, AST_BINARY_SUB, $1, $3, NULL); }
    | expr '*' expr           { $$ = AstNode_create(yylineno, AST_BINARY_MUL, $1, $3, NULL); }
    | expr '/' expr           { $$ = AstNode_create(yylineno, AST_BINARY_DIV, $1, $3, NULL); }
    | expr '%' expr           { $$ = AstNode_create(yylineno, AST_BINARY_MOD, $1, $3, NULL); }
    | expr T_IS_EQ expr       { $$ = AstNode_create(yylineno, AST_BINARY_IS_EQ, $1, $3, NULL); }
    | expr T_IS_NOT_EQ expr   { $$ = AstNode_create(yylineno, AST_BINARY_IS_NOTEQ, $1, $3, NULL); }
    | expr '<' expr           { $$ = AstNode_create(yylineno, AST_BINARY_IS_LT, $1, $3, NULL); }
    | expr '>' expr           { $$ = AstNode_create(yylineno, AST_BINARY_IS_GT, $1, $3, NULL); }
    | expr T_LOGICAL_AND expr { $$ = AstNode_create(yylineno, AST_BINARY_AND, $1, $3, NULL); }
    | expr T_LOGICAL_OR  expr { $$ = AstNode_create(yylineno, AST_BINARY_OR, $1, $3, NULL); }
    | '(' expr ')'            { $$ = $2; }
    ;

scalar:
    T_INTEGER_LITERAL                             { $$ = AstNode_create_value($1); }
    | T_DOUBLE_LITERAL                            { $$ = AstNode_create_value($1); }
    | T_BOOLEAN_LITERAL                           { $$ = AstNode_create_value($1);  }
    ;

id:
    T_IDENTIFIER                                  { $$ = AstNode_create_id(yytext); }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stdout, "%s in line %d.\n", s, yylineno);
}

/*
start   -> program
program -> T_LET declaration_list T_IN stmt_list T_END
declaration_list -> declaration_list declaration | ""
declaration -> T_VAR id ":" typename ";"
typename -> T_INT_TYPE | T_BOOL_TYPE
stmt_list -> stmt_list stmt | ""
stmt -> "{" stmt_list "}" | expr ";" | T_PRINT "(" expr ")" ";" | if_stmt | while_stmt
if_stmt -> T_IF "(" expr ")" stmt | T_IF "(" expr ")" stmt T_ELSE stmt
while_stmt -> T_WHILE "(" expr ")" stmt
expr -> scalar | id "=" expr | expr T_BINARY_OP expr | "(" expr ")"
scalar -> T_INTEGER_LITERAL | T_TRUE | T_FALSE
id -> T_IDENTIFIER
 */