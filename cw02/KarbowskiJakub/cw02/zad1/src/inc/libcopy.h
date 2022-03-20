#ifndef JK_02_01_LIBCOPY_H
#define JK_02_01_LIBCOPY_H

#if !defined(IMPL_SYS) && !defined(IMPL_LIB)
#define IMPL_LIB
#endif

int copy_file(const char *in_path, const char *out_path);

#endif
