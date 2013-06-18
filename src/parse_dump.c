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

    printf("# Objs: %ld\n", xrListLen(obj_list));
    
    XR obj = root;
    XR objtable = val_vtable(obj);
    assert(objtable);
    
    /* Lookup the "foo" method */
    XR foo_s = xr_sym("foo");
    assert(val_vtable(foo_s) == symbol_vt);
    XR foo_c = send(objtable, s_at, foo_s);
    assert(foo_c);
    XR foo_m = xrClosureAt(foo_c, 0);
    assert(foo_m);
    printf("foo_m: %ld\n", foo_m);

    /* xr_run_method(foo_m); */

    struct XRMethod *m = (struct XRMethod *) foo_m;

    #if 1

    int oplen = m->code.len;
    int llen = xrListLen(m->locals);
    int vlen = xrListLen(m->values);
    printf("Method stats.\n");
    printf("Opcodes: %d = [%d x %ld = %ld bytes\n", oplen, oplen, sizeof(XR_OP), oplen * sizeof(XR_OP));
    printf("Locals: %d = [%d x %ld = %ld bytes\n", llen, llen, sizeof(XR), llen * sizeof(XR));
    printf("Values: %d = [%d x %ld = %ld bytes\n", vlen, vlen, sizeof(XR), vlen * sizeof(XR));

    XR_OP *p = m->code.ops;
    size_t i;
    for (i = 0; i < m->code.len; i++) {
        printf("%ld: \t%s\t", i, op_info[p[i].code].name);
        switch (p[i].code) {
            case OP_IVAL:
                {
                    XR val = xrListAt(m->values, p[i].a);
                    XR str = xrSafeLit(val);
                    printf("%d (%s)", p[i].a, xrStrPtr(str));
                    break;
                }
            case OP_LSTORE: case OP_LLOAD:
                printf("%d (%s)", p[i].a, xrSymPtr(xrListAt(m->locals, p[i].a)));
                break;
            case OP_JMP: case OP_NOTJMP:
                printf("%d", p[i].a + 1);
                break;
            case OP_DIV: case OP_MULT: case OP_PLUS: case OP_MINUS:
                break;
            default:
                printf("%d, %d", p[i].a, p[i].b);
        }
        printf("\n");
    }

    fprintf(stderr, "###### Running 'foo' #####\n");
    xr_run_method(m);
    #endif

    return 0;
}
