#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char **argv)
{
    xr_init();

    bool eval = false;
    bool verbose = false;

    int c;
    while ((c = getopt(argc, argv, "ev")) != -1) {
        switch (c) {
        case 'e':
            eval = true;
            break;
        case 'v':
            verbose = true;
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
        FILE *fp = fopen("elixr.edb", "w");
        qsend(root, "pack", fp);
        fclose(fp);
        return 0;
    }

    if (verbose)
        log("##############\nRunning VM\n##############\n");

    xr_run_method(init_m);

    return 0;
}
