#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>

#include <common/slg.h>
#include <common/hash.h>

#include <parser/ast.h>
#include <parser/parser.h>
#include <parser/compile.h>
#include <parser/error.h>

//Symbol *sym;

CompilerContext context;
extern FILE *yyin;

static inline void show_help()
{
    printf("Usage: slgc [operations] [options] <file>\n");
    printf("If ran without options, slgc will compile the given source file into slgvm binary.\n");
    printf("Operations:\n");
    printf("    -S, --skip-assemble\n");
    printf("        Stop after the stage of compilation proper; do not assemble.\n");
    printf("    -A, --assemble-only\n");
    printf("        Assemble the given SLGVM assembly text file into SLGVM binary.\n");
    printf("    -D, --disassemble\n");
    printf("        Disassemble the given binary and write the result to the standard output.\n");
    printf("    -h, --help\n");
    printf("        Show this help message.\n");
    printf("Options:\n");
    printf("    -o <outfile>, --output <outfile>\n");
    printf("        Output results into outfile.\n");
    printf("    -v, --verbose\n");
    printf("        Do not use.\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    enum slgc_program_mode {
        SLGC_COMPILE_FULL,
        SLGC_SKIP_ASSEMBLE,
        SLGC_ASSEMBLE,
        SLGC_DISASSEMBLE,
        SLGC_HELP
    };

    enum slgc_program_mode mode = SLGC_COMPILE_FULL;

    static struct option long_options[] = {
        {"assemble",        no_argument,        0, 'A'},
        {"disassemble",     no_argument,        0, 'D'},
        {"skip-assemble",   no_argument,        0, 'S'},
        {"output",          required_argument,  0, 'o'},
        {"verbose",         no_argument,        0, 'v'},
        {"help",            no_argument,        0, 'h'},
    };

    char *out_fname = NULL;

    while (true) {
        int option_index = 0;
        int c = getopt_long (argc, argv, "SADhvo:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'S':
                mode = SLGC_SKIP_ASSEMBLE;
                break;
            case 'A':
                mode = SLGC_ASSEMBLE;
                break;
            case 'D':
                mode = SLGC_DISASSEMBLE;
                break;
            case 'o':
                out_fname = optarg;
            case 'h':
                mode = SLGC_HELP;
                break;
            default:
                exit(1);
                break;
        }
    }

    if (mode == SLGC_HELP) {
        show_help();

        return EXIT_SUCCESS;
    }

    context.symbol_table = SymbolTable_create(NULL);

    if (optind >= argc) {
        fprintf(stderr, "slgc: No input file given. Exiting.\n");

        return EXIT_FAILURE;
    }

    const char *input_fname = argv[1];
    FILE *infp = fopen(input_fname, "r");

    struct stat stbuffer;
    if (!stat(input_fname, &stbuffer) == 0) {
        print_error("No such file or directory");
        exit(EXIT_FAILURE);
    }

    // if (out_fname == NULL) {
    //     out_fname  = "a.out";
    // }

    // FILE *tmpfp = fmemopen(NULL, 1000, "r");


    FILE *tmpfp = fopen("result.s", "w");

    yyin = infp;

    yyparse();

    compile(context.ast_root, tmpfp);

    fclose(tmpfp);

    // for (size_t i = 0; i < vm.plen; i++) {
    //     fwrite(&vm.program[i], sizeof(slgvm_ir), 1, outfp);
    // }

    AstNode_destroy(context.ast_root);
    SymbolTable_destroy(context.symbol_table);

    return EXIT_SUCCESS;
}
