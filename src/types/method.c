#include <string.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "method.h"

#include "symbol.h"
#include "xrstring.h"
#include "closure.h"

XR method_name(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return ((struct XRMethod*)self)->name;
}

XR method_literal(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return xr_str("(method)");
}

XR method_values(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->values;
}

XR method_locals(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->locals;
}

XR method_scope_locals(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->running_scope.locals;
}

XR method_args(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->args;
}

/* TODO: use AST#show or something similar instead of explicit switch */
/* Also hopefully ramp up the homoiconicity by using some kind of tuple structure
 * for ASTS */
/* Sort out what goes to stderr/stdout and shit */
XR method_show(XR cl, XR self)
{
    (void) cl;

    struct XRMethod *m = xrMthd(self);

    int oplen = m->code.len;
    int llen = xrListLen(m->locals);
    int vlen = xrListLen(m->values);
    log("Method: "); log(xrSymPtr(m->name)); log("\n");
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

    return VAL_NIL;
}

XR method_pack(XR cl, XR self, FILE *fp)
{
    assert(self);

    fwrite("M", 1, 1, fp);
    //char *str = "test method packing";
    //fwrite(str, strlen(str), 1, fp);

    struct XRMethod *m = (struct XRMethod *) self;
    struct XRAsm *code = (struct XRAsm *) &(m->code);

    assert(m);
    assert(code);

    char *msym = xrSymPtr(m->name);
    fprintf(stderr, "Method (%s) has %d opcodes.\n", msym, code->len);

    fwrite(&(code->len), sizeof(code->len), 1, fp);
    fwrite(&(code->ops), sizeof(*(code->ops)), code->len, fp);
}

XR method_unpack(FILE *fp)
{
    XR self = malloc(sizeof(struct XRMethod));
    //FIXME: move to XRList of opcodes?
}

void xr_method_methods()
{
    qdef_method(method_vt, "name", method_name);
    qdef_method(method_vt, "literal", method_literal);
    qdef_method(method_vt, "locals", method_locals);
    qdef_method(method_vt, "scope_locals", method_scope_locals);
    qdef_method(method_vt, "pack", method_pack);
    qdef_method(method_vt, "unpack", method_unpack);

    qdef_method(method_vt, "values", method_values);
    qdef_method(method_vt, "args", method_args);
    qdef_method(method_vt, "show", method_show);
}
