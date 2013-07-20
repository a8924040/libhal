/*
**  ************************************************************************
**
**  File    Name  : Type.h
**  Description   : 常用宏定义
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __TYPE_H__
#define __TYPE_H__
#ifdef __cplusplus
extern "C" {
#endif

    typedef unsigned char               U8;
    typedef unsigned short              U16;
    typedef unsigned int                U32;
    typedef unsigned long long          U64;

    typedef char                        S8;
    typedef signed short                S16;
    typedef signed int                  S32;
    typedef signed long long            S64;

    typedef unsigned char              *PU8;
    typedef unsigned short             *PU16;
    typedef unsigned int               *PU32;
    typedef unsigned long long         *PU64;

    typedef signed char                *PS8;
    typedef signed short               *PS16;
    typedef signed int                 *PS32;
    typedef signed long long           *PS64;

#ifdef __cplusplus
}
#endif

#endif

