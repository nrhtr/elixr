#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "types.h"
#include "util.h"

XR xr_alloc(size_t size)
{
    struct XRTable **ppvt = (struct XRTable **) calloc(1, sizeof(struct XRTable *) + size);
    return (XR)(ppvt + 1);
}
