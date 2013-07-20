############################################################################
#
#  for libdvr
#
# (C) Copyright 1992 to 2005 ZHEJIANG DaHua Technology Stock CO., LTD.
#
# sh4-linux version 1.0
#
############################################################################
ifeq ($(plat), dm8168)
	MARCH=dm8168
	MCROSS=arm-none-linux-gnueabi-
	MDEVICE=DVR
endif

ifeq ($(plat), st7108)
	MARCH=st7108
	MCROSS=sh4-linux-uclibc-
	MDEVICE=DVR
endif

ifeq ($(plat), dm368)
	MARCH=dm368
	MCROSS=arm-none-linux-gnueabi-
	MDEVICE=PVR
endif

ifeq ($(plat), hi35xx)
	MARCH=HI35XX
	MCROSS=arm-hisiv200-linux-
	MDEVICE=DVR
endif

ifeq ($(plat), dm8107)
	MARCH=dm8107
	MCROSS=arm-linux-uclibc-
	MDEVICE=DVR
	CFLAGS = -DN56_DVR
endif

ifeq ($(plat), dm8147)
	MARCH=dm8147
	MCROSS=arm-linux-uclibc-
	MDEVICE=DVR
	CFLAGS = -DN56_DVR
endif
ifeq ($(plat), s3c2440)
	MARCH=S3C2440
	MCROSS=arm-linux-
	MDEVICE=DVR
endif




