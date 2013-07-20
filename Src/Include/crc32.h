/*
**  ********************************************************************************
**                                     DVRDEF
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2008, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**  File        : crc32.h
**  Description :
**  Modify      : 2008-10-09        nils        Create the file
**  ********************************************************************************
*/
#ifndef _CRC32_H_
#define _CRC32_H_
extern const unsigned int crc32_table[256];

/* ========================================================================= */
#define DO1(buf) crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

    inline unsigned int crc32(unsigned int crc, const void *ss, int len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC32_H_ */


