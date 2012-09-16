#ifndef OBJECT_MODEL_H
#define OBJECT_MODEL_H

#ifdef DMALLOC
#include <dmalloc.h>
#endif

typedef unsigned long XR;
typedef XR (*val_f)(XR, XR, ...);

XR bind(XR rcv, XR msg);
XR object_new(XR parent, XR name);
void object_add_method(XR obj, XR meth);

void xr_root_methods(void);

#endif /* OBJECT_MODEL_H */
