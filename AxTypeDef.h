// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_AxTypeDef_H__
#define RCSGREP_AxTypeDef_H__

#include <bits/types.h>

typedef __uint32_t      AXINDEX_T; 
typedef unsigned char   AXBYTE_T;
typedef __uint64_t      AXMilliSec;

#endif

//    ---------- [[ reference ]] ----------
//    #include <bits/types.h>
//
//    /* Convenience types.  */
//    typedef unsigned char                         __u_char;
//    typedef unsigned short int                    __u_short;
//    typedef unsigned int                          __u_int;
//    typedef unsigned long int                     __u_long;
//    
//    /* Fixed-size types, underlying types depend on word size and compiler.  */
//    typedef signed char                           __int8_t;
//    typedef unsigned char                         __uint8_t;
//    typedef signed short int                      __int16_t;
//    typedef unsigned short int                    __uint16_t;
//    typedef signed int                            __int32_t;
//    typedef unsigned int                          __uint32_t;
//    #if __WORDSIZE == 64
//    typedef signed long int                       __int64_t;
//    typedef unsigned long int                     __uint64_t;
//    #elif defined __GLIBC_HAVE_LONG_LONG
//    __extension__ typedef signed long long int    __int64_t;
//    __extension__ typedef unsigned long long int  __uint64_t;
//    #endif
