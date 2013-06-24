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

    struct XRList *list = xrAsList(self);

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

XR xr_list_at(XR cl, XR self, XR pos)
{
    (void) cl;

    if (!xrIsNum(pos) || xrInt(pos) >= xrListLen(self) || xrInt(pos) < 0)
        return VAL_NIL;

    return xrListAt(self, xrInt(pos));
}

XR xr_list_put(XR cl, XR self, XR pos, XR item)
{
    (void) cl;

    if (!xrIsNum(pos) || xrInt(pos) >= xrListLen(self) || xrInt(pos) < 0)
        return VAL_NIL;

    xrListAt(self, xrInt(pos)) = item;

    return self;
}

XR xr_list_len(XR cl, XR self)
{
    (void) cl;

    return xrNum(xrListLen(self));
}

void xr_list_methods()
{
    def_method(list_vt, s_literal, list_literal);
    def_method(list_vt, s_string, list_string);

    /* FIXME: just testing out crap */
    /*def_method(list_vt, xr_sym("at"), xr_list_at);*/
    def_method(list_vt, s_at, xr_list_at);
    def_method(list_vt, s_put, xr_list_put);
    def_method(list_vt, xr_sym("len"), xr_list_len);
    def_method(list_vt, xr_sym("append"), list_append);
    /*def_method(list_vt, s_put, xr_list_put);*/
}
