#ifndef METHOD_H
#define METHOD_H

#include <stdio.h>

XR method_pack(XR cl, XR self, FILE *fp);

XR xr_method_name(XR cl, XR self);
XR xr_method_literal(XR cl, XR self);
XR xr_method_values(XR cl, XR self);
XR xr_method_locals(XR cl, XR self);
XR xr_method_args(XR cl, XR self);

void xr_method_methods(void);

#endif
