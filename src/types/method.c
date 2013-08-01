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

XR method_args(XR cl, XR self)
{
    (void) cl;

    return ((struct XRMethod *)self)->args;
}

void xr_method_methods()
{
    qdef_method(method_vt, "name", method_name);
    qdef_method(method_vt, "literal", method_literal);
    qdef_method(method_vt, "locals", method_locals);
    qdef_method(method_vt, "values", method_values);
    qdef_method(method_vt, "args", method_args);
}
