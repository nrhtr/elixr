#ifndef LIST_H
#define LIST_H

void xr_list_methods(void);

XR list_alloc(unsigned long size);
XR list_empty(void);
XR list_new(XR v);
XR list_new_len(unsigned long len);
XR list_build(unsigned int n, ...);

/* Messages */
XR list_append(XR cl, XR self, XR item);
XR list_literal(XR cl, XR self);

#endif
