#include <stdio.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

void print_method_stats(struct XRMethod *m)
{
    int oplen = m->code.len;
    int llen = xrListLen(m->locals);
    int vlen = xrListLen(m->values);
    log("Method stats.\n");
    log("Opcodes: %d\t= [%3d x %ld = %3d bytes]\n", oplen, oplen, sizeof(XR_OP), oplen * sizeof(XR_OP));
    log("Locals:  %d\t= [%3d x %ld = %3d bytes]\n", llen, llen, sizeof(XR), llen * sizeof(XR));
    log("Values:  %d\t= [%3d x %ld = %3d bytes]\n", vlen, vlen, sizeof(XR), vlen * sizeof(XR));

    XR_OP *p = m->code.ops;
    size_t i;
    for (i = 0; i < m->code.len; i++) {
        log("%ld.\t%s\t", i, op_info[p[i].code].name);
        switch (p[i].code) {
            case OP_IVAL:
                {
                    XR val = xrListAt(m->values, p[i].a);
                    XR str = xrSafeLit(val);
                    log("%d\t(%s)", p[i].a, xrStrPtr(str));
                    break;
                }
            case OP_LSTORE: case OP_LLOAD:
            {
                XR lsym;
                if (p[i].a < xrListLen(m->args))
                    lsym = xrListAt(m->args, p[i].a);
                else
                    lsym = xrListAt(m->locals, p[i].a - xrListLen(m->args));
                log("%d\t(%s)", p[i].a, xrSymPtr(lsym));
                break;
            }
            case OP_JMP: case OP_NOTJMP:
                log("%d", p[i].a + 1);
                break;
            case OP_DIV: case OP_MULT: case OP_PLUS: case OP_MINUS:
                break;
            default:
                log("%d, %d", p[i].a, p[i].b);
        }
        log("\n");
    }
}

int main(int argc, char *argv[])
{
    xr_init();

    if (argc > 1)
        stdin = freopen(argv[1], "r", stdin);

    if (!stdin)
        return 1;

    XR obj_list = xr_parse_dump_from_stdin();

    log("# Objs: %ld\n", xrListLen(obj_list));
    
    XR obj = root;
    XR objtable = val_vtable(obj);
    assert(objtable);
    
    /* Lookup the "init" method */
    XR init_c = send(objtable, s_at, xr_sym("init"));
    XR init_m = xrClosureAt(init_c, 0);

    struct XRMethod *m = (struct XRMethod *) init_m;

    print_method_stats(m);

    xr_run_method(m);

    return 0;
}
