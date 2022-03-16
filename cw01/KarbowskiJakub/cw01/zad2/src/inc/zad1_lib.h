#ifndef JK_ZAD2_SRC_INC_ZAD1_LIB_H
#define JK_ZAD2_SRC_INC_ZAD1_LIB_H

/*
 * Library loading mechanism:
 * 1. If compiled without ZAD1_LIB_DLL, expects
 *    the linker to link to static/shared lib.
 * 2. If compiled with ZAD1_LIB_DLL, will attempt
 *    loading the shared lib during runtime:
 *    - will first try OS paths looking for ZAD1_LIB_FILE
 *    - fill fallback to hardcoded ZAD1_LIB_PATH absolute path
 */

#ifdef ZAD1_LIB_DLL

int zad1_lib_load();

#endif

#endif
