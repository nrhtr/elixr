#include <stdlib.h>
#include <stdio.h>

#include "internal.h"
#include "elixr.h"

#include "closure.h"
#include "method.h"
#include "xrstring.h"
#include "symbol.h"
#include "table.h"

XR closure_new(val_f method, int dlen)
{
    struct XRClosure *closure = malloc(sizeof(struct XRClosure) + dlen * sizeof(XR));
    closure->method = method;
    closure->native = 1;
    closure->mt = closure_vt;

    return (XR)closure;
}

XR closure_literal(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return xr_str("(closure)");
}

XR closure_get_method(XR cl, XR self)
{
    (void) cl;

    if (((struct XRClosure*)self)->native == 1)
        assert(0 && "Trying to get method on native closure");

    return xrClosureAt(self, 0);
}

XR closure_pack(XR cl, XR self, FILE *fp)
{
    (void) cl;

    struct XRClosure *c = (struct XRClosure *) self;
    fprintf(stderr, "Packing closure.\n");
    fprintf(stderr, "\tmethod = %p\n", c->method);
    fprintf(stderr, "\tdata[0] = %p\n", xrClosureAt(self, 0));
    fprintf(stderr, "\tnative = %d\n", c->native);

    int native = c->native;

    if (native) {
        fseek(fp, 1, SEEK_CUR);
    } else { 
        method_pack(0, xrClosureAt(self, 0), fp);
    }
}

void xr_closure_methods()
{
    def_method(closure_vt, s_literal, closure_literal);
    qdef_method(closure_vt, "get_m", closure_get_method);

    qdef_method(closure_vt, "pack", closure_pack);
}
