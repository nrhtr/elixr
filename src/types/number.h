#ifndef NUMBER_H
#define NUMBER_H

#include <stdio.h>

void xr_number_methods(void);

XR xr_double(double d);
XR number_unpack(FILE *fp);
XR number_pack(XR cl, XR self, FILE *fp);
XR number_string(XR cl, XR self);

XR double_unpack(FILE *fp);
XR double_pack(XR cl, XR self, FILE *fp);

// VM Math methods

XR number_add(XR cl, XR self, XR num);
XR number_sub(XR cl, XR self, XR num);
XR number_mul(XR cl, XR self, XR num);
XR number_div(XR cl, XR self, XR num);

#endif
