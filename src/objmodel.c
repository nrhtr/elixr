#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "dbpack.h"
#include "types.h"

/*
XR xr_bind(XR rcv, XR msg)
{
    struct vtable * vt = ((struct XRSymbol*)rcv)->mt;

    struct XRClosure *closure;
    closure = ((msg == s_lookup) && (rcv == (XR)vtable_vt))
        ? (struct XRClosure *)vtable_lookup(0, vt, msg)
        : (struct XRClosure *)send(vt, s_lookup, msg);

    return closure;
}
*/



XR xr_send(XR a, XR msg, XR b)
{
    if (a == VAL_NIL)
        return VAL_NIL;

    if (!xrIsPtr(a))
        return VAL_NIL;

    return send(a, msg, b);
}

XR call_method_args(XR cl, XR self, int argc, XR *argv)
{
    struct XRClosure *c = cl;

    switch (argc) {
        case 0:
            return c->method(cl, self);
        case 1:
            return c->method(cl, self, argv[0]);
        case 2:
            return c->method(cl, self, argv[0], argv[1]);
        case 3:
            return c->method(cl, self, argv[0], argv[1], argv[2]);
        case 4:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3]);
        case 5:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4]);
        case 6:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5]);
        case 7:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6]);
        case 8:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7]);
        case 9:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8]);
        case 10:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9]);
        case 11:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]);
        case 12:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11]);
        case 13:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],
                    argv[12]);
        case 14:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],
                    argv[12], argv[13]);
        case 15:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],
                    argv[12], argv[13], argv[14]);
    }
}

XR bind(XR rcv, XR msg)
{
    /* TODO: Use proper message-sending lookups */

    XR vt = 0;

    /* FIXME: We do this instead of just boxing everything with a reference to its vtable.
     * Is that reasonable?
     */
    if (rcv == VAL_NIL) {
        vt = nil_vt;
    } else if (rcv == VAL_TRUE || rcv == VAL_FALSE) {
        vt = bool_vt;
    } else if (xrIsPtr(rcv)) {
        vt = xrMTable(rcv);
    } else if (xrIsNum(rcv)) {
        vt = num_vt;
    } else {
        fprintf(stderr, "FIXME: sort out primitive type VTs.\n");
    }

    assert(vt);
    assert(msg);

//if (msg == s_lookup && object == 
    XR m = table_at(0, vt, msg);
    if (m == VAL_NIL) {
        printf("Method lookup %s failed.\n", xrSymPtr(msg));
    }

    return m;
}

void object_add_closure(XR obj, XR cl)
{
    assert(cl);
    //assert(0);

    XR m = xrClosureAt(cl, 0);
    XR nm = ((struct XRClosure*)cl)->data[0];

    char *mname = xrSymPtr(xrMethName(m));

    fprintf(stderr, "Added closure (%s) to object.\n", mname);
    fprintf(stderr, "m = %p\n", m);

    assert(nm);
    assert(m);


    send(xrMTable(obj), s_put, xrMethName(m), cl);
}

XR xr_obj_symbol(XR cl, XR self)
{
    (void) cl;

    return xrObj(self)->name;
}

XR object_new(XR parents, XR name)
{
    struct XRObject *obj = malloc(sizeof(struct XRObject));
    obj->mt = xr_table_empty();
    obj->parents = parents;
    obj->name = name;

    return (XR)obj;
}

XR xr_root_vt(XR cl, XR self)
{
    (void) cl;

    return xrMTable(self);
}

void xr_root_methods(void)
{
    def_method(xrMTable(root), s_symbol, xr_obj_symbol);

    /* TODO: sort out object model/cloning/lobby system */
    qdef_method(xrMTable(root), "vt", xr_root_vt);
    qdef_method(xrMTable(root), "pack", db_pack_object);
}
