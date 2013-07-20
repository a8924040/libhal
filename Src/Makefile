############################################################################
#
# Makefile for libbase_2440
#
# 
#
# 2011-11-24		Tango		File created
# 
# 
#
############################################################################
PWD = $(shell pwd)
LIBDVR_BASE_DIR     = $(PWD)/..
LIBDVR_BUILD_DIR    = $(LIBDVR_BASE_DIR)/Build
LIBDVR_INER_INCLUDE = $(PWD)/Include
LIBDVR_EXTERN_INCLUDE = $(LIBDVR_BASE_DIR)/Include
LIBDVR_DEST_DIR     = $(LIBDVR_BASE_DIR)/Lib/$(MARCH)
include $(LIBDVR_BUILD_DIR)/config.mk
include $(LIBDVR_BUILD_DIR)/platform.mk

MY_CFLAGS   = -static -I$(LIBDVR_INER_INCLUDE)  -I$(LIBDVR_EXTERN_INCLUDE)
MY_LIBS   := -lc 
SRCDIRS += ./Common	 \
					./Uart 	\
					./NetWork \
					./IniFile \
					./IR       \
					./GPIO     \
					./FeedBuffer \
					./GPS       \
					./IIC        \
					./System     \
					./Mouse       \
					./Boot     \
					./UsbInfo     \
					./Flash       \
					./Wifi   \
					./SMTP

LIBNAME = libhal.a
include $(LIBDVR_BUILD_DIR)/Rules.mak

