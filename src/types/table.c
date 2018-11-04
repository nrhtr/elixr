#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <assert.h>

#include "internal.h"

#include "objmodel.h"
#include "elixr.h"
#include "symbol.h"
#include "xrstring.h"
#include "dbpack.h"
#include "table.h"
#include "closure.h"
#include "number.h"
#include "types/list.h"

#include "dbpack.h"

#if 0
#define bucket_index(table, hash) ((hash) & (xrTblAlloc(table) - 1))
#else
#define bucket_index(table, hash) ((hash) % (xrTblAlloc(table)))
#endif

XR xr_table_alloc(size_t desired)
{
    size_t alloc = 1;
    while (alloc < desired)
        alloc <<= 1;

    struct XRTable *new = malloc(sizeof(struct XRTable) + sizeof(struct dbt_bucket) * alloc);
    new->mt = table_vt;
    new->alloc = alloc;
    new->len = 0;

    /* Use memset */
    size_t i;
    for (i = 0; i < alloc; i++) {
        new->buckets[i].key = VAL_NIL;
        new->buckets[i].next = 0;
    }

    assert(new->alloc > 0);

    return (XR)new;
}

XR xr_table_empty(void)
{
    return xr_table_alloc(32);
}

XR xr_table_with_one(XR k, XR v)
{
    XR t = xr_table_empty();
    table_put(0, t, k, v);

    return t;
}

/* TODO: move matches into list head */
XR table_at(XR cl, XR self, XR key)
{
    (void) cl;

    assert(self);
    assert(key);

    
    if (xrIsPtr(key) && xrMTable(key) != symbol_vt) {
        printf("Cannot use as a table key: %lu\n", key);
        assert(0);
        printf("VT: %lu\n", xrMTable(key));
        printf("SYMVT: %lu\n", symbol_vt);
        printf("STRVT: %lu\n", string_vt);
        return VAL_NIL;
    }

    unsigned int hash = xrHash(key);
    size_t pos = bucket_index(self, hash);
    struct dbt_bucket *first = xrTblBuckets(self) + pos;

    assert(first);

    while (first != 0) {
        /* TODO: compare keys with message sending? 
         * Probably not necessary. Make only builtin strings/sym/num hashable?
         * Leave it flexible for now, optimise later.
        */
        if (first->key != VAL_NIL && sym_num_eq(first->key, key) == VAL_TRUE) {
            return (XR)first->val;
        }
        first = first->next;
    }
    
    return VAL_NIL;
}

/* TODO: check for overloaded table and resize as needed, return new table */
XR table_put(XR cl, XR self, XR key, XR val)
{
    (void) cl;

    assert(key);
    assert(val);
    assert(self);

    if (xrIsPtr(key) && xrMTable(key) != symbol_vt) {
        return VAL_FALSE;
    }

    unsigned int hash = xrHash(key);
    size_t pos = bucket_index(self, hash);
    struct dbt_bucket *tgt = xrTblBuckets(self) + pos;

    while (tgt != NULL) {
        if (tgt->key != VAL_NIL && sym_num_eq(tgt->key, key) == VAL_TRUE) {
            tgt->val = val;
            return VAL_TRUE;
        }
        tgt = tgt->next;
    }

    tgt = xrTblBuckets(self) + pos;
    /* First record of the chain is stored in the array itself */
    if (tgt->key == VAL_NIL) {
        tgt->key = key;
        tgt->val = val;
        xrTblSize(self)++;
        return VAL_TRUE;
    }

    /* We need to allocate a new node */
    struct dbt_bucket *it = malloc(sizeof(struct dbt_bucket));
    if (!it) {
        puts("Malloc failed.\n");
        return VAL_FALSE;
    }

    /* Insert new node just after the in-struct header */
    it->key = key;
    it->val = val;
    it->next = tgt->next;
    tgt->next = it;
    xrTblSize(self)++;

    return VAL_FALSE;
}

XR table_literal(XR cl, XR self)
{
    (void) cl;

    XR string = xr_str_empty();

    string = xr_str_append_chars(0, string, "{");

    int first = true;
    xrTblEach(self, key, val, ({
        if (!first) {
            string = xr_str_append_chars(0, string, ", ");
        }

        string = xr_str_append(0, string, xrSafeLit(key));
        string = xr_str_append_chars(0, string, " => ");
        string = xr_str_append(0, string, xrSafeLit(val));

        first = false;
    }));

    string = xr_str_append_chars(0, string, "}");
    return string;
}

XR table_pack(XR cl, XR self, FILE *fp)
{
    (void) cl;
    
    fwrite("T", 1, 1, fp);

    unsigned long size = xrTblSize(self);

    fprintf(stderr, "Packing table of size %d.\n", size);

    if (fwrite(&size, sizeof(size), 1, fp) != 1) {
        fprintf(stderr, "Failed to write table header");
        return VAL_FALSE;
    }

    xrTblEach(self, key, val, {
    	fprintf(stderr, "\t(%d => %d)\n", key, val);
        if (xrIsNum(key))
            number_pack(0, key, fp);
        else
            send(key, s_pack, fp);

        if (xrIsNum(val))
            number_pack(0, val, fp);
        else
            send(val, s_pack, fp);
    });

    return self;
}

XR table_unpack(FILE *fp)
{
    XR self = xr_table_empty();

    char c;
    fread(&c, sizeof(char), 1, fp);
    if (c != 'T') {
        fprintf(stderr, "Invalid table header.\n");
        fprintf(stderr, "pos: %d.\n", ftell(fp));
	assert(0);
        return VAL_NIL;
    }

    unsigned long size;
    int read = fread(&size, sizeof(size), 1, fp);
    if (read != 1) {
        fprintf(stderr, "Failed reading size of table entry.\n");
        exit(1);
    }

    unsigned long x;
    fprintf(stderr, "Unpacking table with size %ld\n", size);
    fprintf(stderr, "pos: %d.\n", ftell(fp));
    for (x = 0; x < size; x++) {
        XR a, b;

        a = data_unpack(fp);
        b = data_unpack(fp);
        table_put(0, self, a, b);
    }

    return self;
}

XR table_show(XR cl, XR self)
{ 
    (void) cl;

    int first = 1;
    printf("{");
    xrTblEach(self, key, val, {
        if (!first) {
            printf(", ");
        }
        
        send(xrSafeLit(key), s_show);
        printf(" => ");
        send(xrSafeLit(val), s_show);

        first = 0;
    });
    printf("}");

    return self;
}

XR table_dump(XR cl, XR self)
{
    (void) cl;

    int i;
    for (i = 0; i < xrTblAlloc(self); i++) {
        struct dbt_bucket *b = xrTblBuckets(self) + i;
        printf("[");
        while (b) {
            if (b->key) {
                send(xrSafeLit(b->key), s_show);
                printf("[%ld]=>(%ld), ", b->key, xrHash(b->key));
            } 
            b = b->next;
        }
        printf("]\n");
    }

    return VAL_NIL;
}

XR table_raw_dump(XR cl, XR self)
{
    (void) cl;

    int i;
    for (i = 0; i < xrTblAlloc(self); i++) {
        struct dbt_bucket *b = xrTblBuckets(self) + i;
        printf("[");
        while (b) {
            if (b->key) {
                XR key = b->key;
                XR val = b->val;
                printf("[%lu]", key);

                printf(" => ");
                /*if (xrIsPtr(val))*/
                    /*printf("[%s]", xrSymPtr(val));*/
                /*else*/
                    printf("[%lu]", val);
            }
            b = b->next;
        }
        printf("]\n");
    }

    return VAL_NIL;
}

XR table_keys(XR cl, XR self)
{
    (void) cl;

    XR lst = list_empty();

    xrTblForeach(self, key, val) {
        list_append(0, lst, key);
    }

    return lst;
}

XR table_values(XR cl, XR self)
{
    (void) cl;

    XR lst = list_empty();

    xrTblForeach(self, key, val) {
        list_append(0, lst, val);
    }

    return lst;
}

void xr_table_methods()
{
    // Once we've installed s_put, we can use message sending to install the rest
    table_put(0, table_vt, s_put, def_closure(table_put));

    qdef_method(table_vt, "at", table_at); //table_put(0, table_vt, s_at, def_closure(table_at));

    qdef_method(table_vt, "show",   table_show);
    qdef_method(table_vt, "literal", table_literal);
    qdef_method(table_vt, "pack",    table_pack);
    qdef_method(table_vt, "unpack",  table_unpack);

    qdef_method(table_vt, "keys", table_keys);
    qdef_method(table_vt, "values", table_values);
}
