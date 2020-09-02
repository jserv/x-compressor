#ifndef LIBX_H_
#define LIBX_H_

#include <stddef.h>

void x_init();
void *x_compress(void *iptr, size_t isize, void *optr);
void *x_decompress(void *iptr, size_t isize, void *optr);

#endif
