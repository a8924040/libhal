/*
**  ************************************************************************
**
**  File    Name  : CommonDef.h
**  Description   : 常用宏定义
**  Modification  : 2011/11/24   tango_zhu   创建
**  ************************************************************************
*/
#ifndef __COMMONDEF_H__
#define __COMMONDEF_H__
#ifdef __cplusplus
extern "C" {
#endif

#define CROL(value, bits) ((value << bits) | (value >> (8*sizeof(int) - bits)))
#define CROR(value, bits) ((value >> bits) | (value << (8*sizeof(int) - bits)))




    /* 打印字体显示的颜色 */
#define BLANK          "\033[m"
#define RED           "\033[0;32;31m"
#define GREEN         "\033[0;32;32m"
#define BLUE          "\033[0;32;34m"
#define DARY_GRAY     "\033[1;30m"
#define CYAN          "\033[0;36m"
#define PURPLE        "\033[0;35m"
#define BROWN         "\033[0;33m"
#define YELLOW        "\033[1;33m"
#define WHITE         "\033[1;37m"



#define offset_member(TYPE, MEMBER) ((char *) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offset_member(type,member));})


#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )

#define UPSTRING(pstr)   do{                             \
        char *ptr=pstr;               \
        while(*ptr)                    \
        {                               \
            *ptr=toupper(*ptr);       \
            ptr++;                        \
        }                                 \
    }while(0)

#define LOWERSTRING(pstr) do{                             \
        char *ptr=pstr;               \
        while(*ptr)                    \
        {                               \
            *ptr=tolower(*ptr);       \
            ptr++;                        \
        }                                \
    }while(0)

#ifdef __cplusplus
}
#endif

#endif
