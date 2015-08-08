#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "internal.h"

#include "elixr.h"
#include "xrstring.h"
#include "list.h"
#include "number.h"
#include "table.h"
#include "symbol.h"
#include "closure.h"

XR list_alloc(unsigned long size)
{
    struct XRList *lst = malloc(sizeof(struct XRList));
    lst->alloc = size;
    lst->len = 0;
    lst->mt = list_vt;
    lst->data = malloc(size * sizeof(XR));

    return (XR)lst;
}

XR list_new_len(unsigned long size)
{
    XR list = list_alloc(size);
    xrListLen(list) = size;

    return list;
}

XR list_pack(XR cl, XR self, FILE *fp)
{
    xrListEach(self, index, item, {
        qsend(item, "pack", fp);
    });
}

XR list_empty()
{
    return list_alloc(1);
}

XR list_new(XR v)
{
    XR list = list_alloc(1);

    xrListHead(list) = v;
    xrListLen(list) = 1;

    return list;
}

XR list_build(unsigned int n, ...)
{
    XR list = list_empty();

    va_list va;
    va_start(va, n);

    while (n--) {
        list = list_append(0, list, va_arg(va, XR));
    }

    va_end(va);

    return list;
}

/////// Messages ///////

XR list_dump(XR self, FILE *fp)
{
    unsigned long len = xrListLen(self);

    fwrite(&len, sizeof(unsigned long), 1, fp);
    fwrite(xrListData(self), sizeof(unsigned long), len, fp);

    return self;
}

XR list_append(XR cl, XR self, XR item)
{
    (void) cl;

    struct XRList *list = xrList(self);

    const unsigned delta = 4;

     if (list->len + 1 > list->alloc) {
        size_t new_size = list->alloc * 2 + delta;
        list->data = realloc(list->data, new_size * sizeof(XR));
        list->alloc = new_size;
    }

    list->len++;
    list->data[list->len - 1] = item;
    
    return self;
}

XR list_literal(XR cl, XR self)
{
    (void) cl;

    if (xrListLen(self) == 0)
        return xr_str("[]");

    XR lit = xr_str_empty();

    lit = xr_str_append_chars(0, lit, "[");

    xrListEach(self, index, item, {
        XR item_str = xrSafeLit(item);
        lit = xr_str_append(0, lit, item_str);

        if (index != xrListLen(self) - 1) {
            lit = xr_str_append_chars(0, lit, ", ");
        }
    });

    lit = xr_str_append_chars(0, lit, "]");
    return lit;
}

XR list_string(XR cl, XR self)
{
    (void) cl;

    XR lit = xr_str_empty();

    lit = xr_str_append_chars(0, lit, "[");

    xrListEach(self, index, item, {
        XR item_str = (xrIsNum(item) ? number_string(0, item) : send(item, s_string)); 
        lit = xr_str_append(0, lit, item_str);

        if (index != xrListLen(self) - 1) {
            lit = xr_str_append_chars(0, lit, ", ");
        }
    });

    lit = xr_str_append_chars(0, lit, "]");
    return lit;
}

XR list_at(XR cl, XR self, XR pos)
{
    (void) cl;

    if (!xrIsNum(pos) || xrInt(pos) >= xrListLen(self) || xrInt(pos) < 0)
        return VAL_NIL;

    return xrListAt(self, xrInt(pos));
}

XR list_put(XR cl, XR self, XR pos, XR item)
{
    (void) cl;

    if (!xrIsNum(pos) || xrInt(pos) >= xrListLen(self) || xrInt(pos) < 0)
        return VAL_NIL;

    xrListAt(self, xrInt(pos)) = item;

    return self;
}

XR list_len(XR cl, XR self)
{
    (void) cl;

    return xrNum(xrListLen(self));
}

XR list_mktable(XR cl, XR self)
{
    if (xrListLen(self) % 2 != 0)
        return VAL_NIL;

    XR tbl = xr_table_empty();

    for (int i = 0; i < xrListLen(self); i += 2)
    {
        XR key = xrListAt(self, i);
        XR val = xrListAt(self, i+1);

        table_put(0, tbl, key, val);
    }

    return tbl;
}

void xr_list_methods()
{
    def_method(list_vt, s_literal, list_literal);
    def_method(list_vt, s_string, list_string);
    def_method(list_vt, s_at, list_at);
    def_method(list_vt, s_put, list_put);

    def_method(list_vt, xr_sym("len"), list_len);
    def_method(list_vt, xr_sym("append"), list_append);
    def_method(list_vt, xr_sym("mktable"), list_mktable);
}
