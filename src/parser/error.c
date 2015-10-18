#include <stdio.h>
#include <stdlib.h>

#include "parser/compile.h"

#define FMT_RESET       "\033[0m"
#define FMT_BLACK       "\033[30m"      /* Black */
#define FMT_RED         "\033[31m"      /* Red */
#define FMT_GREEN       "\033[32m"      /* Green */
#define FMT_YELLOW      "\033[33m"      /* Yellow */
#define FMT_BLUE        "\033[34m"      /* Blue */
#define FMT_MAGENTA     "\033[35m"      /* Magenta */
#define FMT_CYAN        "\033[36m"      /* Cyan */
#define FMT_WHITE       "\033[37m"      /* White */
#define FMT_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define FMT_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define FMT_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define FMT_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define FMT_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define FMT_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define FMT_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define FMT_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

extern CompilerContext context;

void trigger_error(int lineno, const char *msg, ...)
{
    va_list args;

    fprintf(stderr, FMT_BOLDRED "error" FMT_RESET " in line %d: ", lineno);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");

    AstNode_destroy(context.ast_root);
    SymbolTable_destroy(context.symbol_table);

    fprintf(stderr, "Compilation failed.\n");
    exit(EXIT_FAILURE);
}

void trigger_warning(int lineno, const char *msg, ...)
{
    va_list args;

    fprintf(stderr, FMT_BOLDMAGENTA "warning" FMT_RESET " in line %d: ", lineno);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void print_fatal(const char *msg, ...)
{
    va_list args;

    fprintf(stderr, FMT_BOLDRED "fatal error: " FMT_RESET);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void print_error(const char *msg, ...)
{
    va_list args;

    fprintf(stderr, FMT_BOLDRED "error: " FMT_RESET);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#undef FMT_RESET
#undef FMT_BLACK
#undef FMT_RED
#undef FMT_GREEN
#undef FMT_YELLOW
#undef FMT_BLUE
#undef FMT_MAGENTA
#undef FMT_CYAN
#undef FMT_WHITE
#undef FMT_BOLDBLACK
#undef FMT_BOLDRED
#undef FMT_BOLDGREEN
#undef FMT_BOLDYELLOW
#undef FMT_BOLDBLUE
#undef FMT_BOLDMAGENTA
#undef FMT_BOLDCYAN
#undef FMT_BOLDWHITE
