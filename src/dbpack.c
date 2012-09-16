#include <stdio.h>

#include "internal.h"
#include "elixr.h"

#include "types.h"

XR data_unpack(FILE *fp)
{
    char type;
    XR data = VAL_NIL;

    if (fread(&type, 1, 1, fp) != 1) {
        return VAL_NIL;
    }

    switch (type) {
        case ':':
            data = symbol_unpack(fp);
            break;
        case 'N':
            data = number_unpack(fp);
            break;
        case 'D':
            data = double_unpack(fp);
            break;
        case 'S':
            data = xr_str_unpack(fp);
            break;
        default:
            printf("%c", type);
            break;
    }

    return data;
}
