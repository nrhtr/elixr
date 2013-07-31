#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>

void xr_table_methods(void);

XR xr_table_unpack(FILE *fp);
XR xr_table_empty (void);
XR xr_table_alloc (size_t desired_size);
XR xr_table_with_one (XR k, XR v);

/* Messages */
XR table_dump     (XR cl, XR self);
XR table_raw_dump (XR cl, XR self);
XR table_at       (XR cl, XR self, XR key);
XR table_put      (XR cl, XR self, XR key, XR val);
XR table_print    (XR cl, XR self);

XR table_pack     (XR cl, XR self, FILE *fp);

#endif
