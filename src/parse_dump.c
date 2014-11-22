#include <stdio.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char *argv[])
{
    xr_init();

    XR obj_list;

    if (argc > 1) {
        char *file = argv[1];
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


    return 0;
}
