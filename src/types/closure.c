#include <stdlib.h>
#include <stdio.h>

#include "internal.h"
#include "elixr.h"

#include "closure.h"
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

void xr_closure_methods()
{
    def_method(closure_vt, s_literal, closure_literal);
    qdef_method(closure_vt, "get_m", closure_get_method);
}
