#include "vm/slgvm.h"
#include "vm/slgasm.h"
#include "vm/slgdisasm.h"

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

static inline void show_help()
{
    printf("Usage: slgvm [options] file\n");
    printf("If ran without options, slgvm will execute the SLGVM binary in the argument file.\n");
    printf("Options:\n");
    printf("    -d, --disassemble\n");
    printf("        Disassemble the given binary and write the result to the standard output.\n");
    printf("    -a, --assemble\n");
    printf("        Assemble the given SLGVM assembly text file into SLGVM binary.\n");
    printf("    -v, --verbose\n");
    printf("        Do not use.\n");
    printf("    -h, --help\n");
    printf("        Show this help message.\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    enum slgvm_program_mode {
        Assemble,
        Disassemble,
        Execute,
        Help
    };

    enum slgvm_program_mode mode = Execute;

    static struct option long_options[] = {
        {"assemble",        no_argument, 0, 'a'},
        {"disassemble",     no_argument, 0, 'd'},
        {"verbose",         no_argument, 0, 'v'},
        {"help",            no_argument, 0, 'h'},
    };

    while (true) {
        int option_index = 0;
        int c = getopt_long (argc, argv, "adhv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'a':
                mode = Assemble;
                break;
            case 'd':
                mode = Disassemble;
                break;
            case 'h':
                mode = Help;
                break;
            default:
                exit(1);
                break;
        }
    }

    if (mode == Help) {
        show_help();
        return 0;
    }

    if (optind >= argc) {
        fprintf(stderr, "[SLGVM] No input. Stop.\n");
        return 1;
    }

    const char *path = argv[optind];
    FILE *fp = fopen(path, "rb");

    slgvm_core vm;

    if (mode == Execute) {
        slgvm_load_program(&vm, fp);
        slgvm_execute(&vm);
    } else if (mode == Disassemble) {
        slgvm_core vm;
        slgvm_load_program(&vm, fp);
        slgasm_disassemble(vm.program, vm.plen, stdout);
    } else if (mode == Assemble) {
        if (argc <= optind + 1) {
            fprintf(stderr, "[SLGVM] Assembly usage: slgvm -a <input> <output>\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        FILE *out_file = fopen(argv[optind + 1], "wb");
        slgasm_assemble(fp, vm.program, &vm.plen);

        for (size_t i = 0; i < vm.plen; i++) {
            fwrite(&vm.program[i], sizeof(slgvm_ir), 1, out_file);
        }

        fclose(out_file);
    }


    return 0;
}

