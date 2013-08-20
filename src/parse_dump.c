#include <stdio.h>

#include "internal.h"
#include "elixr.h"
#include "compile.h"
#include "vm.h"
#include "types.h"

int main(int argc, char *argv[])
{
    xr_init();

    if (argc > 1)
        stdin = freopen(argv[1], "r", stdin);

    if (!stdin)
        return 1;

    xr_parse_expr_from_stdin();

    return 0;
}
