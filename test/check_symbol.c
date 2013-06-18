#include <check.h>
#include <stdio.h>

#include "../src/internal.h"
#include "../src/objmodel.h"
#include "../src/elixr.h"
#include "../src/types.h"

START_TEST (test_symbol_new)
{
    XR sym = xr_sym("test string");

    fail_unless(sym);
}
END_TEST

/* Interning not entirely sorted out yet. Not sure
 * how it will all work */
/* UPDATE: Interning now based on a global khash table */
START_TEST (test_symbol_intern)
{
    XR a = xr_sym("identical");
    XR b = xr_sym("identical");

    fail_unless(a == b);
}
END_TEST

START_TEST (test_symbol_literal)
{
	XR sym = xr_sym("symbol");
	XR lit = send(sym, s_literal);

	fail_unless(qsend(lit, "eq", xr_str(":symbol")) == VAL_TRUE);
}
END_TEST

START_TEST (test_symbol_string)
{
    XR sym = xr_sym("symbol");
    XR str = send(sym, s_string);

    fail_unless(qsend(str, "eq", xr_str("symbol")) == VAL_TRUE);
}
END_TEST

Suite *symbol_suite(void)
{
    Suite *s = suite_create("Symbol");

    TCase *tc_core = tcase_create("Core");
    
    tcase_add_test(tc_core, test_symbol_new);
    tcase_add_test(tc_core, test_symbol_intern);
    tcase_add_test(tc_core, test_symbol_literal);
    tcase_add_test(tc_core, test_symbol_string);

    suite_add_tcase(s, tc_core);

    return s;
}
