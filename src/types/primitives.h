#ifndef PRIMITIVES_H
#define PRIMITIVES_H

XR xr_bool_string(XR cl, XR self);
XR xr_bool_symbol(XR cl, XR self);
XR xr_nil_string(XR cl, XR self);
XR xr_nil_symbol(XR cl, XR self);
XR xr_nil_nil(XR cl, XR self);

void xr_primitive_methods(void);

#endif
