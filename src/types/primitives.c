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
	def_method(bool_vt, s_string,  xr_bool_string);
	def_method(bool_vt, s_symbol,  xr_bool_symbol);

	def_method(nil_vt, s_string,  xr_nil_string);
	def_method(nil_vt, s_symbol,  xr_nil_symbol);
	qdef_method(nil_vt, "nil",  xr_nil_nil);
}
