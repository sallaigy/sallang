%{
    #include <stdlib.h>

    #include "ast.h"
    #include "parser.h"

    extern void yyerror(const char *s);
%}

DIGIT           [0-9]

%option yylineno

%x IN_COMMENT

%%


{DIGIT}+        {
                    yylval.sval.type = SVALUE_INTEGER;
                    yylval.sval.ival = atof(yytext);

                    return T_INTEGER_LITERAL;
                }


{DIGIT}+"."{DIGIT}+ {
                        yylval.sval.type = SVALUE_DOUBLE;
                        yylval.sval.dval = atof(yytext);

                        return T_DOUBLE_LITERAL;
                    }

"true"          {
                    yylval.sval.type = SVALUE_BOOLEAN;
                    yylval.sval.ival = 1;

                    return T_BOOLEAN_LITERAL;
                }

"false"         {
                    yylval.sval.type = SVALUE_BOOLEAN;
                    yylval.sval.ival = 0;

                    return T_BOOLEAN_LITERAL;
                }

"&&"            {
                    return T_LOGICAL_AND;
                }

"||"            {
                    return T_LOGICAL_OR;
                }

"=="            {
                    return T_IS_EQ;
                }

"!="            {
                    return T_IS_NOT_EQ;
                }

"func"          {
                    return T_FUNC;
                }

"let"           {
                    return T_LET;
                }

"in"            {
                    return T_IN;
                }

"end"           {
                    return T_END;
                }

"var"           {
                    return T_VAR;
                }

"const"         {
                    return T_CONST;
                }

"if"            {
                    return T_IF;
                }

"else"          {
                    return T_ELSE;
                }

"while"         {
                    return T_WHILE;
                }

"print"         {
                    return T_PRINT;
                }

"int"           {
                    return T_INT_TYPE;
                }

"double"        {
                    return T_DOUBLE_TYPE;
                }

"bool"          {
                    return T_BOOL_TYPE;
                }


[-+*/%()=:;{}<>!] {
                    return *yytext;
                }

[a-z][a-z0-9]*  {
                    return T_IDENTIFIER;
                }

<INITIAL>{
    "/*"              BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
    "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
    "*"       // eat the lone star
    \n        yylineno++;
}

[ \t\n\r]       ;


.               { yyerror("Unknown character"); }

%%

int yywrap(void)
{
    return 1;
}
