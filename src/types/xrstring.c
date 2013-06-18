#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "internal.h"
#include "elixr.h"

#include "table.h"
#include "xrstring.h"
#include "symbol.h"
#include "closure.h"

XR xr_str_alloc(int size)
{
    struct XRString *new = malloc(sizeof(struct XRString) + size*sizeof(char));

    new->len = 0;
    new->alloc = size;
    new->mt = string_vt;

    return (XR) new;
}

XR xr_str_empty(void)
{
    return xr_str_alloc(0);
}

XR xr_strn(const char *str, size_t len)
{
    struct XRString *s = xrAsStr(xr_str_alloc(len + 1));
    s->len = len;
    memcpy(s->chars, str, len);
    s->chars[len] = '\0';
    
    return (XR) s;
}

XR xr_str(const char *str)
{
    return xr_strn(str, strlen(str));
}

XR xr_str_intern(XR cl, XR self)
{
    (void) cl;

    return xr_sym(xrStrPtr(self));
}

/* TODO: Eliminate extra malloc. Use macros to do it all in one go */
XR xr_str_sprintf(const char *fmt, ...)
{
    char *buf;
    unsigned int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    buf = malloc(len);

    va_start(ap, fmt);
    vsnprintf(buf, len, fmt, ap);
    va_end(ap);

    buf[len - 1] = '\0';

    XR str = xr_str(buf);
    free(buf);
    return str;
}

XR xr_str_from_num(XR cl, XR num)
{
    (void) cl;

    /*
    char buffer[20];

    sprintf(buffer, "%ld", xrInt(num));
    */
    return xr_str_sprintf("%ld", xrInt(num));
}

XR xr_str_length(XR cl, XR self)
{
    (void) cl;

    return xrNum(((struct XRString *)self)->len);
}

XR xr_str_print(XR cl, XR self)
{
    (void) cl;

    printf("%.*s", (int)xrStrLen(self), xrStrPtr(self));
    fflush(stdout);

    return self;
}

XR xr_str_println(XR cl, XR self)
{
    (void) cl;

    printf("%.*s\n", (int)xrStrLen(self), xrStrPtr(self));

    /* TODO: should println return anything? means you can do stuff like "" showln showln  */
    return self;
}

XR xr_str_string(XR cl, XR self)
{
    (void) cl;

    return self;
}

/* Even if we later make strings immutable, we'll probably still want a
 * comparison function since we might not intern everything */
XR xr_str_eq(XR cl, XR self, XR other)
{ 
    (void) cl;

    return (strcmp(xrStrPtr(self), xrStrPtr(other)) == 0 ? VAL_TRUE : VAL_FALSE);
}

XR xr_str_literal(XR cl, XR self)
{
    (void) cl;

    XR build = xr_str_empty();

    build = xr_str_append_chars(0, build, "\"");
    build = xr_str_append(0, build, self);
    build = xr_str_append_chars(0, build, "\"");

    return (XR)build;
}

/* TODO: ditch verbose macros and just access XRString * explicitly */
XR xr_str_append(XR cl, XR self, XR other)
{
    (void) cl;

    unsigned long self_l = xrStrLen(self), other_l = xrStrLen(other);

    XR new = xr_str_alloc(self_l + other_l + sizeof('\0'));
    memcpy(xrStrPtr(new), xrStrPtr(self), xrStrLen(self));
    memcpy(xrStrPtr(new) + self_l, xrStrPtr(other), xrStrLen(other));

    xrStrLen(new) = self_l + other_l;
    xrStrPtr(new)[xrStrLen(new)] = '\0';

    return new;
}

XR xr_str_pack(XR cl, XR self, FILE *fp)
{
    (void) cl;

    unsigned long len = xrStrLen(self);
    fwrite("S", 1, 1, fp);
    fwrite(&len, sizeof(len), 1, fp);
    fwrite(xrStrPtr(self), sizeof(char), len, fp);

    return self;
}

/* Messagify with an XRFile of some sort, see TODO notes */
XR xr_str_unpack(FILE *fp)
{
    unsigned long len;
    fread(&len, sizeof(len), 1, fp);
    XR str = xr_str_alloc(len);
    int success = fread(xrStrPtr(str), len, 1, fp);
    assert(success);
    xrStrLen(str) = len;

    return str;
}

XR xr_str_append_chars(XR cl, XR self, const char *extra)
{
    (void) cl;

    XR str = xr_str(extra);

    return xr_str_append(0, self, str);
}

XR xr_str_append_chars_n(XR cl, XR self, const char *extra, size_t len)
{
    (void) cl;

    XR str = xr_strn(extra, len);

    return xr_str_append(0, self, str);
}

XR xr_str_add(XR cl, XR self, XR other)
{
    (void) cl;
#if 1
    /* Only add strings for now */
    if (!xrIsPtr(other) || val_vtable(other) != string_vt)
        return VAL_NIL;
#else
    if (xrIsNum(other)) {
        return xr_str_sprintf("%s%ld", xrStrPtr(self), xrInt(other));
    }
#endif

    XR str = send(other, s_string);
    return xr_str_append(0, self, str);
}

void xr_string_methods()
{
    /* Predefined */
    def_method(string_vt, s_string,  xr_str_string);
    def_method(string_vt, s_print,   xr_str_print);
    def_method(string_vt, s_literal, xr_str_literal);
    def_method(string_vt, s_pack,    xr_str_pack);
    def_method(string_vt, s_unpack,  xr_str_unpack);
    def_method(string_vt, s_add,      xr_str_add);

    qdef_method(string_vt, "show",  xr_str_print);
    qdef_method(string_vt, "showln",  xr_str_println);
    qdef_method(string_vt, "length",  xr_str_length);
    qdef_method(string_vt, "append", xr_str_append);
    qdef_method(string_vt, "eq",      xr_str_eq);
    qdef_method(string_vt, "intern",  xr_str_intern);
}
