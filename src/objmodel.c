#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
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

XR bind(XR rcv, XR msg)
{
    /* TODO: Use proper message-sending lookups */

    XR vt = 0;

    /* TODO: Is there a cleaner way to handle message sends
     * on primitives/fixnums ? */
    if (rcv == VAL_NIL) {
        vt = nil_vt;
    } else if (rcv == VAL_TRUE || rcv == VAL_FALSE) {
        vt = bool_vt;
    } else if (xrIsPtr(rcv)) {
        vt = val_vtable(rcv);
    } else if (xrIsNum(rcv)) {
        vt = num_vt;
    } else {
        fprintf(stderr, "FIXME: sort out primitive type VTs.\n");
    }

    assert(vt);
    assert(msg);

    XR m = xr_table_at(0, vt, msg);
    if (m == VAL_NIL) {
        printf("Method lookup %s failed.\n", xrSymPtr(msg));
    }

    return m;
}


void object_add_method(XR obj, XR method)
{
    struct XRMethod *m = (struct XRMethod *)method;
    send(val_vtable(obj), s_put, m->name, m);
}

/* testing this out */
XR obj_symbol_tmp(XR cl, XR self)
{
    (void) cl;

    return xrAsObj(self)->name;
}

XR object_new(XR parent, XR name)
{
    struct XRObject *obj = malloc(sizeof(struct XRObject));
    obj->mt = xr_table_empty();
    obj->parent = parent;
    obj->name = name;

    return (XR)obj;
}

XR xr_root_list(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return list_empty();
}

XR xr_root_vt(XR cl, XR self)
{
    (void) cl;

    return val_vtable(self);
}

void xr_root_methods(void)
{
    def_method(val_vtable(root), s_symbol, obj_symbol_tmp);

    /* FIXME: sort out object model/cloning/lobby system */
    qdef_method(val_vtable(root), "List", xr_root_list);
    qdef_method(val_vtable(root), "vt", xr_root_vt);
}
