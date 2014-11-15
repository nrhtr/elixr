#include <check.h>
#include <stdlib.h>

#include "../src/internal.h"
#include "../src/objmodel.h"
#include "../src/elixr.h"

#include "../src/types.h"

START_TEST (test_table_empty)
{
    XR t = xr_table_empty();

    fail_unless(t);
}
END_TEST

START_TEST (test_table_put_get)
{
    XR t = xr_table_empty();

    XR key = xr_sym("key");
    XR val = xr_sym("val");

    send(t, s_put, key, val);

    XR res = send(t, s_at, key);

    fail_unless(res == val);
}
END_TEST

START_TEST (test_table_nil_result)
{
    XR t = xr_table_empty();

    XR key = xr_sym("key");
    XR val = xr_sym("val");

    send(t, s_put, key, val);

    XR not = xr_sym("notakey");

    XR res = send(t, s_at, not);

    fail_unless(res == VAL_NIL);
}
END_TEST

START_TEST (test_table_valid_key)
{
    XR t = xr_table_empty();

    XR key = list_build(2, xr_sym("a"), xr_sym("b"));
    XR val = xr_sym("val");

    int res = send(t, s_put, key, val);
    fail_unless(res == VAL_FALSE);
}
END_TEST

START_TEST (test_table_collisions)
{
    XR t = xr_table_empty();

    int i;
    for (i = 0; i < 32; i++) {
        char buf[32];
        sprintf(buf, "sym%d", i);
        send(t, s_put, xrNum(i), xrNum(i+7));
    }

    /*xr_table_raw_dump(0, t);*/

    fail_unless(send(t, s_at, xrNum(7)) == xrNum(14));

    /*
    xrTblForeach(t, k, v) {
        send(k, s_print); printf(" => "); send(xrSafeLit(v), s_print); puts("");
    }
    xrTblEach(t, k, v, {
       send(k, s_print); printf(" => "); send(xrSafeLit(v), s_print); puts("");
    });
    */
}
END_TEST

START_TEST (test_table_literal)
{
    XR t = xr_table_empty();

    XR a = xr_sym("key_a");
    XR b = xr_sym("key_b");

    send(t, s_put, a, xrNum(42));
    send(t, s_put, b, xrNum(24));

    fail_unless(xr_str_eq(0, send(t, s_literal), xr_str("{:key_a => 42, :key_b => 24}")));
}
END_TEST



Suite *table_suite(void)
{
    Suite *s = suite_create("Table");

    TCase *tc_core = tcase_create("Core");
    
    tcase_add_test(tc_core, test_table_empty);
    tcase_add_test(tc_core, test_table_put_get);
    tcase_add_test(tc_core, test_table_collisions);
    tcase_add_test(tc_core, test_table_literal);
    tcase_add_test(tc_core, test_table_nil_result);
    tcase_add_test(tc_core, test_table_valid_key);

    suite_add_tcase(s, tc_core);

    return s;
}
