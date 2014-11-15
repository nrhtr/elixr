#include <check.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "types.h"

START_TEST (test_list_create_empty)
{
    XR list = list_empty();

    fail_unless(list);
}
END_TEST

START_TEST (test_list_length)
{
    XR list = list_build(3, xrNum(42), xr_sym("testing"), xr_double(22.7));

    fail_unless(xrListLen(list) == 3);
}
END_TEST

Suite *list_suite(void)
{
    Suite *s = suite_create("List");

    // add core test case
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_list_create_empty);
    tcase_add_test(tc_core, test_list_length);

    suite_add_tcase(s, tc_core);

    return s;
}
