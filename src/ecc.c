#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char *argv[])
{
    int c;
    int execute = 0;
    int verbose = 0;
    char *file = NULL;
    while ((c = getopt(argc, argv, "ev")) != -1) {
        switch (c) {
        case 'e':
            execute = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            printf("%s", "usage");
            return 1;
            file = strdup(optarg);
        }
    }

    xr_init();

    XR obj_list;

    if (file) {
        obj_list = xr_parse_dump_file(file);
    } else {
        obj_list = xr_parse_dump_from_stdin();
    }

    log("# Objs: %ld\n", xrListLen(obj_list));
    
    XR root_mt = xrMTable(root);
    assert(root_mt);
    
    /* Lookup the "init" method */
    XR init_m = xrClosureAt(send(root_mt, s_at, xr_sym("init")), 0);

    qsend(init_m, "show");
    xr_run_method(init_m);

    FILE *fp = fopen("blahtestsymfile", "w");
    qsend(root, "pack", fp);
    fclose(fp);

    return 0;
}
