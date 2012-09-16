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
#endif
