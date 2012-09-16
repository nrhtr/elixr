#include <stdio.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char *argv[])
{
    xr_init();

    int pexpr = 0;

    if (argc > 1)
        stdin = freopen(argv[1], "r", stdin);

    if (argc > 2)
        pexpr = 1;



    if (!stdin)
        return 1;

    /*xr_parse_any_from_stdin();*/
    //return 0;
    /*assert(blah);*/
    /*printf("%p\n", blah);*/
    /*fflush(stdout);*/
    /*//send(blah, s_literal);*/
    /*//XR bs = send(blah, s_string);*/
    /*//send(send(blah, s_string), s_print);*/

    XR obj_list = xr_parse_dump_from_stdin();

    printf("# Objs: %ld\n", xrListLen(obj_list));
    
    /*XR newobj = xrListAt(obj_list, 0);*/
    XR newobj = root;
    XR objtable = val_vtable(newobj);
    assert(objtable);
    

    /*XR ex_foo_s = xr_sym_lookup("foo");*/
    /*printf("Looked up 'foo' sym.\nVal = %s\nVT = %p\nptr = %p\n", xrSymPtr(ex_foo_s), val_vtable(ex_foo_s), ex_foo_s);*/
    /*printf("symbol_vt = %x\n", symbol_vt);*/

    /*val_vtable(ex_foo_s) = symbol_vt;*/
    /*printf("Looked up 'foo' sym.\nVal = %s\nVT = %x\n", xrSymPtr(ex_foo_s), val_vtable(ex_foo_s));*/


    /* Lookup the "foo" method */
    XR foo_s = xr_sym("foo");
    assert(val_vtable(foo_s) == symbol_vt);
    XR foo_m = send(objtable, s_at, foo_s);
    printf("foo_m: %ld\n", foo_m);
    assert(foo_m);

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

    xr_run_method(m);
    #endif

    return 0;
}
