#ifndef DBPACK_H
#define DBPACK_H

#include <stdio.h>

XR data_unpack(FILE *f);
XR db_unpack_object(FILE *f);
XR db_pack_object(XR cl, XR obj, FILE *f);

#endif
