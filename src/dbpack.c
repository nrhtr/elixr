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
            fprintf(stderr, "Unpacking symbol...\n");
            data = symbol_unpack(fp);
            break;
        case 'N':
            fprintf(stderr, "Unpacking number...\n");
            data = number_unpack(fp);
            break;
        case 'D':
            fprintf(stderr, "Unpacking double...\n");
            data = double_unpack(fp);
            break;
        case 'S':
            fprintf(stderr, "Unpacking string...\n");
            data = xr_str_unpack(fp);
            break;
	case 'M':

        default:
            fprintf(stderr, "Attempt to unpack unknown type %c\n", type);
            assert(0);
            break;
    }

    return data;
}

XR db_pack_object(XR cl, XR obj, FILE *fp)
{
    // pack name as string
    // pack parents list
    // pack objvar table
    // pack method table

    char *objname = xrSymPtr(xrObj(obj)->name);
    fprintf(stderr, "Packing object (%s).\n", objname);
    fwrite("O", sizeof(char), 1, fp);
    xr_str_pack(0, xrObj(obj)->name, fp);
    //xr_str_pack(0, xr_str("default"), fp);

    XR parents = xrObjParents(obj);
    if (parents == VAL_NIL) {
        fprintf(stderr, "No parents. FIXME: Packing empty list manually.\n");
        //fwrite("N", 1, 1, fp);
        XR lst = list_empty();
        send(lst, s_pack, fp);

    } else {
        fprintf(stderr, "Packing parents.\n");
        qsend(qsend(xrObjParents(obj), "literal", fp), "showln");
        qsend(xrObjParents(obj), "pack", fp);
    }

    XR vars = xrObjVars(obj);
    if (vars == VAL_NIL) {
        fprintf(stderr, "No vars. FIXME: Packing empty table manually.\n");
        XR lst = xr_table_empty();
        qsend(lst, "pack", fp);
    } else {
        fprintf(stderr, "Packing vars.\n");
        qsend(vars, "pack", fp);
    }

    XR mtable = xrMTable(obj);
    if (mtable == VAL_NIL) {
        fprintf(stderr, "No method table. FIXME: Packing empty table manually.\n");
        XR tbl = xr_table_empty();
        table_pack(0, tbl, fp);
    } else {
        fprintf(stderr, "Packing method table.\n");

        //qsend(mtable, "pack", fp);
        // Pack mtable specially. Normal table_pack would include native methods

        unsigned long size = xrTblSize(mtable);

        fwrite("T", sizeof(char), 1, fp);
        if (fwrite(&size, sizeof(size), 1, fp) != 1) {
            fprintf(stderr, "Failed to write table header");
        }

        xrTblForeach(mtable, key, val) {
            fprintf(stderr, "\t(%d => %d)\n", key, val);

            if (xrClosure(val)->native)
                continue;

            send(key, s_pack, fp);
            send(val, s_pack, fp);
        }
    }
}

XR db_unpack_object(FILE *fp)
{
    assert(fp);

    char c;

    fread(&c, 1, 1, fp);
    if (c != 'O') {
        fprintf(stderr, "Invalid object header.\n");
        return VAL_NIL;
    }

    fread(&c, 1, 1, fp);
    if (c != 'S') {
        fprintf(stderr, "Invalid string header for name.\n");
        return VAL_NIL;
    }

    XR name = xr_str_unpack(fp);

    XR o = object_new(VAL_NIL, name);

    xrObj(o)->parents = list_unpack(fp);
    xrObj(o)->vars = table_unpack(fp);
    xrObj(o)->mt = table_unpack(fp);

    return o;
}

/*
   XR pack_object(XR cl, XR obj, FILE *fp)
   {
// pack name as string
// pack parents list
// pack objvar table
// pack method table
fprintf(stderr, "Packing object.\n");

fwrite("O", sizeof(char), 1, fp);
}
*/
