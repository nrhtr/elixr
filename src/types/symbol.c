#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "elixr.h"
#include "symbol.h"
#include "xrstring.h"
#include "table.h"
#include "closure.h"
#include "khash.h"

KHASH_MAP_INIT_STR(sym, XR);
khash_t(sym) *g_sym_table;

XR sym_lookup(const char *str)
{
    khiter_t k = kh_get(sym, g_sym_table, str);

    if (k != kh_end(g_sym_table)) {
        return kh_val(g_sym_table, k);
    }

    return VAL_NIL;
}


XR symbol_print(XR cl, XR self)
{
    (void) cl;

    printf("%.*s", (int)xrSymLen(self), xrSymPtr(self));
    fflush(stdout);

    return VAL_NIL;
}

XR symbol_println(XR cl, XR self)
{
    (void) cl;

    printf("%.*s\n", (int)xrSymLen(self), xrSymPtr(self));

    return VAL_NIL;
}

void xr_sym_put(XR s)
{
    int ret;
    khiter_t k = kh_put(sym, g_sym_table, xrSymPtr(s), &ret);
    kh_val(g_sym_table, k) = s;
}

XR _xr_sym_alloc(const char *str)
{
    size_t len = strlen(str);
    size_t size = len + sizeof('\0');

    struct XRSymbol *sym = malloc(sizeof(struct XRSymbol) + size*sizeof(char));
    memcpy(sym->chars, str, len);
    sym->chars[len] = '\0';

    sym->len = len;
    sym->alloc = size;
    /* FIXME: change to reflect use of khash */
    sym->hash = kh_str_hash_func(sym->chars);
    /* Just allocing, symbol not necessarily interned */
    sym->interned = 0;
    sym->mt = symbol_vt;

    return (XR) sym;
}

XR _xr_sym_alloc_n(const char *str, size_t len)
{
    size_t size = len + sizeof('\0');

    struct XRSymbol *sym = malloc(sizeof(struct XRSymbol) + size*sizeof(char));
    memcpy(sym->chars, str, len);
    sym->chars[len] = '\0';

    sym->len = len;
    sym->alloc = size;
    /* FIXME: change to reflect use of khash */
    sym->hash = kh_str_hash_func(str);
    /* Just allocing, symbol not necessarily interned */
    sym->interned = 0;
    sym->mt = symbol_vt;

    return (XR) sym;
}

XR xr_sym(const char *str)
{
    XR sym = sym_lookup(str);

    if (sym == VAL_NIL) {
        /* Create a new symbol and intern it in 
         * global symbol table */
        sym = _xr_sym_alloc(str);
        ((struct XRSymbol*)sym)->interned = 1;
        xr_sym_put(sym);
    }
    
    return sym;
}

XR xr_sym_n(const char *str, size_t len)
{
    char buf[64];
    strncpy(buf, str, 64);
    buf[len] = '\0';

    return xr_sym(buf);
}

XR symbol_string(XR cl, XR self)
{
    (void) cl;

    return xr_str(xrSymPtr(self));
}

XR symbol_literal(XR cl, XR self)
{
    (void) cl;

    XR literal = xr_str_sprintf(":%s", xrSymPtr(self));

    return literal;
}

XR symbol_eq(XR cl, XR self, XR other)
{
    (void) cl;

    if (!xrIsPtr(other))
        return VAL_FALSE;

    if (xrMTable(other) != symbol_vt)
        return VAL_FALSE;

    if (xrSymLen(self) != xrSymLen(other))
        return VAL_FALSE;

    /* FIXME: Symbols will all be so short this won't really help, need
     * to be able to just assume all symbols are interned */
    /* UPDATE: all symbols should now be interned. Just need to sort out
     * proper interfaces/messages for comparison */
    if (xrSymInterned(self) && xrSymInterned(other) && self == other) {
        return VAL_TRUE;
    }

    const char *stra = xrSymPtr(self);
    const char *strb = xrSymPtr(other);

    size_t slen = xrSymLen(self);
    size_t olen = xrSymLen(other);

    size_t mlen = (slen < olen) ? slen : olen;

    if (strncmp(stra, strb, mlen) == 0)
        return VAL_TRUE;

    return VAL_FALSE;
}

XR symbol_pack(XR cl, XR self, FILE *fp)
{
    (void) cl;

    unsigned long len = xrSymLen(self);
    assert(fwrite(":", sizeof(char), 1, fp) == 1);
    assert(fwrite(&len, sizeof(len), 1, fp) == 1);
    assert(fwrite(xrSymPtr(self), sizeof(char), len, fp) == len);

    return VAL_TRUE;
}

XR symbol_unpack(FILE *fp)
{
    unsigned long len;
    if (fread(&len, sizeof(len), 1, fp) != 1) {
        fprintf(stderr, "Failed to read symbol length");
        exit(1);
    }

    char *buf = malloc(len);
    
    if (fread(buf, len, 1, fp) != 1) {
        fprintf(stderr, "Failed to read symbol");
        exit(1);
    }

    XR sym = xr_sym_n(buf, len);
    free(buf);
    return sym;
}


/* Used for table comparisons. Can't do fully general comparisons because regular
 * tables are used for interning the symbols needed for message sending.
 */
XR sym_num_eq(XR a, XR b)
{
    if (xrIsNum(a)) {
        if (xrIsNum(b)) {
            return (a == b) ? VAL_TRUE : VAL_FALSE;
        }
        return symbol_eq(0, a, b);
    }
    else
        return symbol_eq(0, b, a);
}

void xr_init_symtable()
{
    g_sym_table = kh_init(sym);
}

/* Init vtable for symbols */
void xr_symbol_methods()
{
    def_method(symbol_vt, s_string,  symbol_string);
    def_method(symbol_vt, s_literal, symbol_literal);
    def_method(symbol_vt, s_print,   symbol_print);

    qdef_method(symbol_vt, "show", symbol_print);
    qdef_method(symbol_vt, "showln", symbol_println);
    qdef_method(symbol_vt, "==",     symbol_eq);
}
