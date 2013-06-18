#ifndef CLOSURE_H
#define CLOSURE_H

void xr_closure_literal(XR cl, XR self);

XR closure_new(val_f method, int dlen);

void xr_closure_methods(void);

#endif
