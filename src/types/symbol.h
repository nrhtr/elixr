#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>

void xr_symbol_methods(void);
void xr_init_symtable(void);
unsigned str_hash(const char *str);

XR sym_lookup(const char *str);
void xr_sym_put(XR s);
XR _xr_sym_alloc(const char *str);
XR _xr_sym_alloc_n(const char *str, size_t len);
XR xr_sym(const char *str);
XR symbol_unpack(FILE *fp);
XR xr_sym_n(const char *str, size_t len);

/* FIXME: sort out the messy comparison functions */
XR sym_num_eq(XR a, XR b);
XR symbol_eq(XR cl, XR self, XR other);

/* Messages */
XR symbol_print(XR cl, XR self);
XR symbol_literal(XR cl, XR self);
XR symbol_string(XR cl, XR self);
XR symbol_hash(XR cl, XR self);

XR symbol_pack(XR cl, XR self, FILE *fp);

#endif
