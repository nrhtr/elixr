#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "internal.h"
#include "elixr.h"
#include "dbpack.h"
#include "vm.h"
#include "types.h"

int main(int argc, char **argv)
{
    xr_init();

    bool verbose = false;

    int c;
    while ((c = getopt(argc, argv, "ev")) != -1) {
        switch (c) {
        case 'v':
            verbose = true;
            break;
        }
    }

    FILE *fp = fopen("dump.db", "r");
    if (!fp) {
        fprintf(stderr, "Error opening dump file.\n");
        return 1;
    }

    XR obj = db_unpack_object(fp);
    assert(obj);

    XR root_mt = xrMTable(obj);
    assert(root_mt);
    
    /* Lookup the "init" method */

    XR name = xrObj(obj)->name;
    qsend(name, "showln");
    qsend(qsend(xrObjVars(obj), "literal"), "showln");

    XR cl = send(root_mt, s_at, xr_sym("init"));
    if (!cl) {
    	fprintf(stderr, "No 'init method found on root.\n");
	return 1;
    }

    XR init_m = xrClosureAt(cl, 0);

    if (verbose)
        qsend(init_m, "show");

    fclose(fp);

    return 0;
}
