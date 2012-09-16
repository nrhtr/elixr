#include <check.h>
#include <stdio.h>
#include "../src/objmodel.h"
#include "../src/internal.h"
#include "../src/elixr.h"
#include "../src/types.h"

Suite *type_suite(void)
{
    Suite *s = suite_create("");
    TCase *tc_core = tcase_create("Types");
    suite_add_tcase(s, tc_core);

    return s;
}
