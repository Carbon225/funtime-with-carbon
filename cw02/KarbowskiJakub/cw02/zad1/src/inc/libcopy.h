#ifndef JK_02_01_LIBCOPY_H
#define JK_02_01_LIBCOPY_H

#if !defined(IMPL_SYS) && !defined(IMPL_LIB)
#define IMPL_LIB
#endif

/**
 * Copy contents of input file
 * into output file
 * skipping empty lines.
 *
 * @param in_path Input file path.
 * @param out_path Output file path.
 * @return 0 or negative error.
 */
int copy_file(const char *in_path, const char *out_path);

#endif
