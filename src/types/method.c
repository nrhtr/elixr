#include "internal.h"
#include "elixr.h"
#include "method.h"

#include "symbol.h"
#include "xrstring.h"
#include "closure.h"

XR xr_method_name(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return ((struct XRMethod*)self)->name;
}

XR xr_method_literal(XR cl, XR self)
{
    (void) cl;
    (void) self;

    return xr_str("(method)");
}

XR xr_method_values(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->values;
}

XR xr_method_locals(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->locals;
}

XR xr_method_args(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->args;
}

void xr_method_methods()
{
    qdef_method(method_vt, "name", xr_method_name);
    qdef_method(method_vt, "literal", xr_method_literal);
    qdef_method(method_vt, "locals", xr_method_locals);
    qdef_method(method_vt, "values", xr_method_values);
    qdef_method(method_vt, "args", xr_method_args);
}
