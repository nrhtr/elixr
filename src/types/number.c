#include <stdio.h>
#include <stdlib.h>

#include "internal.h"

#include "elixr.h"

#include "table.h"
#include "xrstring.h"
#include "symbol.h"
#include "closure.h"

XR xr_double(double num)
{
    struct XRDouble *d = malloc(sizeof (struct XRDouble));
    d->val = num;
    d->mt[-1] = num_vt;
    
    return (XR)d;
}

XR number_string(XR cl, XR self)
{
    (void) cl;

    char buf[40];
    if (xrIsNum(self)) {
        sprintf(buf, "%ld", xrInt(self));
    } else {
        int len = sprintf(buf, "%.16f", ((struct XRDouble *)self)->val);
        while (len > 0 && buf[len - 1] == '0') len--;
        if (buf[len - 1] == '.') len++;
        buf[len] = '\0';
    }

    return xr_str(buf);
}

XR number_add(XR cl, XR self, XR num)
{
    (void) cl;

    if (xrIsNum(self) && xrIsNum(num))
        return xrNum(xrInt(self) + xrInt(num));

    return xr_double(xrDbl(self) + xrDbl(num));
}

XR number_sub(XR cl, XR self, XR num)
{
    (void) cl;

    if (xrIsNum(self) && xrIsNum(num))
        return xrNum(xrInt(self) - xrInt(num));

    return xr_double(xrDbl(self) - xrDbl(num));
}

XR number_mul(XR cl, XR self, XR num)
{
    (void) cl;

    if (xrIsNum(self) && xrIsNum(num))
        return xrNum(xrInt(self) * xrInt(num));

    return xr_double(xrDbl(self) * xrDbl(num));
}

XR number_div(XR cl, XR self, XR num)
{
    (void) cl;

    if (xrIsNum(self) && xrIsNum(num))
        return xrNum(xrInt(self) / xrInt(num));

    return xr_double(xrDbl(self) / xrDbl(num));
}

XR number_pack(XR cl, XR self, FILE *fp)
{
    (void) cl;

    if (xrIsNum(self)) {
        fwrite("N", 1, 1, fp);
        fwrite(&self, sizeof(self), 1, fp);
    } else {
        fwrite("D", 1, 1, fp);
        double val = xrDbl(self);
        fwrite(&val, sizeof(double), 1, fp);
    }

    return self;
}

XR number_unpack(FILE *fp)
{
    XR d;
    fread(&d, sizeof(d), 1, fp);
    
    return d;
}

XR double_unpack(FILE *fp)
{
    double d;
    fread(&d, sizeof(double), 1, fp);

    return xr_double(d);
}

void xr_number_methods(void)
{
#define m(NAME) qdef_method(num_vt, #NAME, number_##NAME)
    m(string);
    m(pack);
#undef m
    qdef_method(num_vt, "literal", number_string);
    qdef_method(num_vt, "+", number_add);
    qdef_method(num_vt, "-", number_sub);
    qdef_method(num_vt, "*", number_mul);
    qdef_method(num_vt, "/", number_div);
}
