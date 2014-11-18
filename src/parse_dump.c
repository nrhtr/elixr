#include <stdio.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char *argv[])
{
    xr_init();

    if (argc > 1)
        stdin = freopen(argv[1], "r", stdin);

    if (!stdin)
        return 1;

    XR obj_list = xr_parse_dump_from_stdin();

    log("# Objs: %ld\n", xrListLen(obj_list));
    
    XR root_mt = xrMTable(root);
    assert(root_mt);
    
    /* Lookup the "init" method */
    XR init_m = xrClosureAt(send(root_mt, s_at, xr_sym("init")), 0);

    qsend(init_m, "show");
    xr_run_method(init_m);


    return 0;
}
