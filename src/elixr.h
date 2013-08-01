#ifndef ELIXIR_H
#define ELIXIR_H

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h>

/* remove */
#include "opcodes.h"
#include "objmodel.h"

void xr_init(void);
int data_eq(XR a, XR b);

/* VTs */
extern XR table_vt, symbol_vt, num_vt, string_vt, closure_vt, list_vt,
ast_vt, bool_vt, nil_vt, method_vt;

/* root obj */
extern XR root;

/* main symbols */
extern XR s_put, s_at, s_def, s_allocate, s_delegated, s_print, s_literal,
s_pack, s_unpack, s_string, s_symbol, s_eq;

/* math */
extern XR s_add, s_sub, s_mul, s_div;

/* Object model stuff
--------------------- */
#define send(OBJ, MSG, ARGS...) ({\
    struct XRClosure *CL = (struct XRClosure *) bind(OBJ, MSG);\
    (CL == VAL_NIL ? VAL_NIL : CL->method((XR)CL, OBJ, ##ARGS));\
})

#define qsend(OBJ, STR, ARGS...) ({\
    struct XRClosure *CL = ((struct XRClosure *) bind(OBJ, xr_sym(STR)));\
    (CL == VAL_NIL ? VAL_NIL : CL->method((XR)CL, OBJ, ##ARGS));\
})

XR xr_send(XR a, XR msg, XR b);

/* Malloc stuff
--------------- */

/* Typecast macros
------------------ */
#define xrTable(v)        ((struct XRTable*)v)
#define xrStr(v)          ((struct XRString*)v)
#define xrObj(v)          ((struct XRObject*)v)
#define xrList(v)         ((struct XRList*)v)
#define xrSym(v)         ((struct XRList*)v)

/* Objmodel macros
------------------ */
#define xrHash(v)         (xrIsNum(v) ? (v) : xrSymHash(v))
#define xrNum(n)          ((XR)((n)<<1) | 1)
#define xrDbl(n)           (xrIsNum(n) ? (double)xrInt(n) : ((struct XRDouble*)n)->val)
#define xrInt(n)          ((long)(n)>>1)
#define xrIsNum(n)       ((n) & 1)
#define xrIsPtr(n)       (!xrIsNum(n))
#define xrIsBool(n)      ((n) & 2)
#define xrSafeLit(i)     (xrIsNum(i) ? xr_str_from_num(0, i) : send(i, s_literal))
#define xrTblCmp(a,b)    (xrIsNum(a) ? (xrIsNum(b) ? (a) == (b) : symbol_eq(0, b, a)) : symbol_eq(0, a, b))
#define xrMkBool(i)         ((i) == 0 ? VAL_FALSE : VAL_TRUE)
#define xrTest(v)         ((v) != VAL_NIL && (v) != VAL_FALSE)

#define xrObjVars(o) (((struct XRObject*)o)->vars)

#define VAL_NIL   ((XR)0)
#define VAL_ZERO  ((XR)1)
#define VAL_FALSE ((XR)2)
#define VAL_TRUE  ((XR)6) 

/* nil   =  00b is_ptr
 * zero  =  01b is_num >> 1 == 0
 * false =  10b is_ptr is_bool
 * true  = 110b is_ptr is_bool
*/

/* Convenience macros
--------------------- */
#define def_closure(FUN)            closure_new((val_f)(FUN), 0)
#define qdef_method(VT, MSG, FN)    send(VT, s_put, xr_sym(MSG), (XR)closure_new((val_f)FN, 0))
#define def_method(VT, SYM, FN)     table_put(0, VT, SYM, (XR)closure_new((val_f)FN, 0))

/* Header macros
---------------- */
#define xrMTable(v)     (((struct XRObject*)v)->mt)

/* List macros
-------------- */
#define xrListLen(l)      (((struct XRList *)(l))->len)
#define xrListAlloc(l)    (((struct XRList *)(l))->alloc)
#define xrListData(l)     (((struct XRList *)(l))->data)
#define xrListAt(l,i)     (((struct XRList *)(l))->data[i])
#define xrListHead(l)     (xrListAt(l, 0))

#define xrListEach(L, N, V, C) \
do { \
    unsigned long N;\
    unsigned long len = xrListLen(L);\
    XR V; \
    for (N = 0; N < len; N++) { \
        V = xrListAt(L, N); \
        C; \
    } \
} while (0)

#define xrListEachR(L, N, V, C) \
do { \
    unsigned long N = xrListLen(L);\
    while (N--) { \
        XR V = xrListAt(L, N); \
        C; \
    } \
} while (0)

#define xrListForeach(L, I) \
for (int n = 0, XR I = xrListAt(L, n); n < xrListLen(L); n++, I = xrListAt(L, n))

#define xrDoFail(S) send(S, s_print); exit(1);

/* Symbol macros
---------------- */
#define xrSymLen(v)      (((struct XRSymbol *)(v))->len)
#define xrSymPtr(v)      (((struct XRSymbol *)(v))->chars)
#define xrSymAlloc(v)    (((struct XRSymbol *)(v))->alloc)
#define xrSymHash(v)     (((struct XRSymbol *)(v))->hash)
#define xrSymInterned(v) (((struct XRSymbol *)(v))->interned)

#define xrStrPtr(S)       (((struct XRString *)(S))->chars)
#define xrStrLen(S)       (((struct XRString *)(S))->len)
#define xrStrAlloc(S)     (((struct XRString *)(S))->alloc)

/* Table macros
---------------- */
#define xrTblIndex(T,I) (xrTable(T)->buckets[I])
#define xrTblSize(T)    (xrTable(T)->len)
#define xrTblAlloc(T)   (xrTable(T)->alloc)
#define xrTblBuckets(T) (xrTable(T)->buckets)

#define xrTblForeach(T, K, V)\
    for (int i = 0; i < xrTblAlloc(T); i++) \
        for (struct dbt_bucket *B = xrTblBuckets(T) + i; B && B->key != VAL_NIL; B = B->next) \
            for (XR K = ((XR)B->key), \
                    V = ((XR)B->val), \
                    n = 0; \
                    n < 1; \
                    n++) 

#define xrTblEach(T, K, V, C) \
do { \
    int i = 0; \
    for (; i < xrTblAlloc(T); i++) { \
        struct dbt_bucket *B = xrTblBuckets(T) + i; \
        for (; B && B->key != VAL_NIL; B = B->next) { \
            XR K = ((XR)B->key); \
            XR V = ((XR)B->val); \
            C; \
        } \
    } \
} while (0)

#define xrClosureAt(C, I) (((struct XRClosure *)C)->data[I])
#define xrMethName(M) (((struct XRMethod *)M)->name)

#if 0
#define XR_HEAD XR mt; char *name;
#else
#define XR_HEAD XR mt;
#endif

struct XRAst {
    XR_HEAD
    unsigned char type;
    XR n[0];
};

struct XRSymbol {
    XR_HEAD
    unsigned long len;
    unsigned long alloc;
    u8 interned;
    unsigned long hash;
    char chars[0];
};

/* Effectively immutable for now */
struct XRString {
    XR_HEAD
    unsigned long len;
    unsigned long alloc;
    char chars[0];
};

struct XRDouble {
    XR_HEAD
    double val;
};

struct XRClosure {
    XR_HEAD
    bool native;

    val_f method;

    XR data[0];
};

struct XRAsm {
    size_t len;
    size_t alloc;
    XR_OP *ops;
};

/* Persistent compiled method on an object */
struct XRMethod {
    XR_HEAD
    XR object;
    XR name;

    /* TODO: change to flexible generic array? */
    struct XRAsm code;

    XR values; /* list of literal values */
    XR locals; /* list of local names as symbols */
    XR args;   /* list of arg names as symbols */
};

struct dbt_bucket {
    XR key;
    XR val;
    struct dbt_bucket *next;
};

struct XRTable {
    XR_HEAD
    int len;
    int alloc;
    struct dbt_bucket buckets[0];
};

/* Can't keep mutable data within the struct as we have
no GC to update on reallocs */
struct XRList {
    XR_HEAD
    unsigned long len;
    unsigned long alloc;
    XR *data;
};

struct XRObject {
    XR_HEAD
    XR parent;
    XR name;
    XR vars;
    int refs;
};

#endif
