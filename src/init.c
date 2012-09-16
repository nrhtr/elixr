#include <stdio.h>
#include "khash.h"

#include "internal.h"
#include "elixr.h"
#include "types.h"

#include "objmodel.h"

XR s_lookup, s_at, s_put, s_def;
XR s_print, s_literal, s_string, s_symbol;
XR s_pack, s_unpack;

XR s_eq, s_neq, s_gt, s_lt, s_gte, s_lte;

XR s_add, s_sub, s_mul, s_div;

/* base vtables are global for now.
 * add base root vt which others delegate from
 * future base/root object for everything
*/
XR table_vt;
XR symbol_vt;
XR num_vt;
XR string_vt;
XR closure_vt;
XR list_vt;
XR ast_vt;
XR bool_vt;
XR nil_vt;
XR method_vt;

XR root;

/* Bootstrap message-sending, etc. */
void xr_init()
{
    /* Init base types */
    xr_init_symtable();

    table_vt = xr_table_empty();
    val_vtable(table_vt) = table_vt;
    /*val_name(table_vt) = "Table_VT";*/

    string_vt   = xr_table_empty(); 
    symbol_vt   = xr_table_empty(); 
    closure_vt  = xr_table_empty(); 
    num_vt      = xr_table_empty(); 
    list_vt     = xr_table_empty(); 
    ast_vt      = xr_table_empty();
    bool_vt     = xr_table_empty();
    nil_vt      = xr_table_empty();
    method_vt   = xr_table_empty();

    root        = object_new(0, xr_sym("root"));

    s_lookup    = xr_sym("lookup");

    s_at        = xr_sym("at");
    s_put       = xr_sym("put");
    s_def       = xr_sym("def");
    s_print     = xr_sym("print");
    s_literal   = xr_sym("literal");
    s_string    = xr_sym("string");
    s_symbol    = xr_sym("symbol");

    s_eq        = xr_sym("==");   

    s_add       = xr_sym("+");
    s_sub       = xr_sym("-");
    s_mul       = xr_sym("*");
    s_div       = xr_sym("/");

    s_pack      = xr_sym("pack");
    s_unpack    = xr_sym("unpack");

    xr_table_methods();
    xr_string_methods();
    xr_symbol_methods();
    xr_primitive_methods();
    xr_closure_methods();
    xr_number_methods();
    xr_list_methods();
    xr_ast_methods();
    xr_root_methods();
    xr_method_methods();

#if 0
        if (val_vtable(key) != symbol_vt) {
            printf("Y U NO SYMBOL\n");
            printf("%s\n", xrSymPtr(key));
            printf("vt: %lu\n", val_vtable(key));
            printf("symvt: %lu\n", symbol_vt);
            printf("strvt: %lu\n", string_vt);
            printf("numvt: %lu\n", num_vt);
            printf("tblvt: %lu\n", table_vt);
            printf("clvt: %lu\n", closure_vt);
            printf("lstvt: %lu\n", list_vt);
            printf("astvt: %lu\n", ast_vt);
            printf("bolvt: %lu\n", bool_vt);
            printf("nilvt: %lu\n", nil_vt);
            printf("rootvt: %lu\n", xrAsObj(root)->mt);
        }
    puts("Dumping table.");
    puts("-------------------------");
    xr_table_dump(0, xrAsObj(root)->string_table);
    puts("");
    puts("-------------------------");
#endif
}
