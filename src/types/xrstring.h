#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include "objmodel.h"

void xr_string_methods(void);

XR xr_str(const char *string);
XR xr_strn(const char *str, size_t len);

XR xr_str_from_num(XR cl, XR x);
XR xr_str_alloc(int size);
XR xr_str_empty(void);
XR xr_str_sprintf(const char *fmt, ...);
XR xr_str_append_chars(XR cl, XR self, const char *extra);
XR xr_str_append_chars_n(XR cl, XR self, const char *extra, size_t n);

XR xr_str_unpack(FILE *fp);


/* Messages */
XR xr_str_length(XR cl, XR self);
XR xr_str_print(XR cl, XR self);
XR xr_str_append(XR cl, XR _self, XR _extra);
XR xr_str_pack(XR cl, XR self, FILE *fp);
XR xr_str_eq(XR cl, XR self, XR other);

#endif
