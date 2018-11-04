#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

void usage()
{
    fprintf(stderr, "Elixr evaluator & compilers\n");
    fprintf(stderr, "usage: ecc [OPTIONS] INFILE\n");
    fprintf(stderr, "-e             evaluate code (default is to compile)\n");
    fprintf(stderr, "-v             turn on verbose output\n");
    fprintf(stderr, "-h             display this help text\n");
    fprintf(stderr, "-o <file>      file to output to when compiling\n");
    fprintf(stderr, "If INFILE is omitted or set to '-' then stdin is used\n");
    exit(1);
}

int main(int argc, char **argv)
{
    xr_init();

    bool eval = false;
    bool verbose = false;
    char *outfile = "elixr.edb";

    int c;
    while ((c = getopt(argc, argv, "ehvo:")) != -1) {
        switch (c) {
        case 'e':
            eval = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            usage();
            break;
        case 'o':
            outfile = optarg;
            break;
        }
    }

    XR obj_list;

    if (optind < argc) {
        char *file = argv[optind];
        obj_list = xr_parse_dump_file(file);
    } else {
        obj_list = xr_parse_dump_from_stdin();
    }

    if (verbose)
        log("# Objs: %ld\n", xrListLen(obj_list));
    
    XR root_mt = xrMTable(root);
    assert(root_mt);
    
    /* Lookup the "init" method */
    XR init_m = xrClosureAt(send(root_mt, s_at, xr_sym("init")), 0);

    if (verbose)
        qsend(init_m, "show");

    if (!eval) {
        FILE *fp = fopen(outfile, "wb");
        qsend(root, "pack", fp);
        fclose(fp);
        return 0;
    }

    if (verbose)
        log("##############\nRunning VM\n##############\n");

    xr_run_method(init_m);

    return 0;
}
