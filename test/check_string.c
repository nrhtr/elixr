#include <check.h>
#include <stdlib.h>

#include "../src/internal.h"
#include "../src/objmodel.h"
#include "../src/elixr.h"

#include "../src/types.h"

START_TEST (test_string_create)
{
    XR str = xr_str("test string");

    fail_unless(str);
}
END_TEST

START_TEST (test_string_length)
{
    XR str = xr_str("test string");

    fail_unless(xrStrLen(str) == 11);
}
END_TEST

START_TEST (test_string_append_str)
{
    XR str1 = xr_str("test ");
    XR str2 = xr_str("string");
    XR str3 = xr_str_append(0, str1, str2);

    fail_unless(xr_str_eq(0, str3, xr_str("test string")) == VAL_TRUE);
}
END_TEST

START_TEST (test_string_create_fmt)
{
    XR str = xr_str_sprintf("%d %d", 1, 2);

    fail_unless(xr_str_eq(0, str, xr_str("1 2")));
}
END_TEST

START_TEST (test_string_append_cstr)
{
    XR str = xr_str("test ");
    str = xr_str_append_chars(0, str, "string");
    
    fail_unless(xr_str_eq(0, str, xr_str("test string")), "String concatenation failed.");
}
END_TEST

Suite *string_suite(void)
{
    Suite *s = suite_create("String");

    // add core test case
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_string_create);
    tcase_add_test(tc_core, test_string_create_fmt);

    tcase_add_test(tc_core, test_string_length);
    tcase_add_test(tc_core, test_string_append_str);
    tcase_add_test(tc_core, test_string_append_cstr);

    suite_add_tcase(s, tc_core);

    return s;
}
