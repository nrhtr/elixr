#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "dbpack.h"
#include "types.h"

XR bind(XR rcv, XR msg)
{
    assert(msg);

    XR vt = 0;
    XR closure = NULL;

    switch (rcv & 3) {
        case 0: // all we know if its either Nil or an object
            if (rcv == VAL_NIL) vt = nil_vt; else vt = xrMTable(rcv);
            break;
        case VAL_FALSE:
        case VAL_TRUE:
            fprintf(stderr, "Looking up method on a BOOL. When does this happen?\n");
            vt = bool_vt;
            break;
        default:
            vt = num_vt;
    }

    if (msg == s_at && rcv == table_vt) {
        closure = table_at(0, rcv, msg);
    } else {
        closure = send(vt, s_at, msg);
    }

    if (closure == VAL_NIL) {
        fprintf(stderr, "Method lookup %s failed.\n", xrSymPtr(msg));
    }

    return closure;
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
        case 16:
            return c->method(cl, self, argv[0], argv[1], argv[2], argv[3], argv[4],
                    argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11],
                    argv[12], argv[13], argv[14], argv[15]);
    }
}

void object_add_closure(XR obj, XR cl)
{
    assert(cl);

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
    struct XRObject *obj = xr_alloc(sizeof(struct XRObject));
    obj->mt[-1] = xr_table_empty();
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
    XR root_vt = xrMTable(root);

    qdef_method(root_vt, "symbol", xr_obj_symbol);
    qdef_method(root_vt, "vt", xr_root_vt);
    qdef_method(root_vt, "pack", db_pack_object);
}
