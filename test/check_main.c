#include <check.h>
#include <stdlib.h>

#include "../src/internal.h"
#include "../src/elixr.h"

Suite *list_suite(void);
Suite *string_suite(void);
Suite *symbol_suite(void);
Suite *table_suite(void);

int main(void)
{
    xr_init();

    int number_failed;
    Suite *s = suite_create("Master");

    SRunner *sr = srunner_create (s);

    srunner_add_suite(sr, list_suite());
    srunner_add_suite(sr, string_suite());
    srunner_add_suite(sr, symbol_suite());
    srunner_add_suite(sr, table_suite());

    srunner_set_fork_status(sr, CK_NOFORK);
    /*srunner_run_all (sr, CK_VERBOSE);*/
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
