#include "internal.h"
#include "elixr.h"

#include "types.h"

XR xr_bool_string(XR cl, XR self)
{
    (void) cl;
    (void) self;

	return (self == VAL_TRUE ? xr_str("true") : xr_str("false"));
}

XR xr_bool_symbol(XR cl, XR self)
{
    (void) cl;
    (void) self;

	return (self == VAL_TRUE ? xr_sym("true") : xr_sym("false"));
}

XR xr_nil_string(XR cl, XR self)
{
    (void) cl;
    (void) self;

	return xr_str("nil");
}

XR xr_nil_symbol(XR cl, XR self)
{
    (void) cl;
    (void) self;

	return xr_sym("nil");
}

XR xr_nil_nil(XR cl, XR self)
{
    (void) cl;
    (void) self;

	return VAL_TRUE;
}

void xr_primitive_methods()
{
#define m(NAME) qdef_method(bool_vt, #NAME, xr_bool_##NAME)
    m(string);
    m(symbol);
#define m(NAME) qdef_method(nil_vt, #NAME, xr_nil_##NAME)
    m(string);
    m(symbol);
#undef m

    qdef_method(nil_vt,  "nil?",     xr_nil_nil);
    qdef_method(bool_vt, "literal",  xr_bool_string);
    qdef_method(nil_vt,  "literal",  xr_nil_string);
}
