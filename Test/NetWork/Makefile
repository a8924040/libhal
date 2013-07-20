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
LIBDVR_BASE_DIR     = $(PWD)/../..
LIBDVR_BUILD_DIR    = $(LIBDVR_BASE_DIR)/Build
LIBDVR_INER_INCLUDE = $(LIBDVR_BASE_DIR)/Src/Include
LIBDVR_EXTERN_INCLUDE = $(LIBDVR_BASE_DIR)/Include

include $(LIBDVR_BUILD_DIR)/config.mk

LIBDVR_DEST_DIR     = $(LIBDVR_BASE_DIR)/Lib/$(MARCH)

MY_CFLAGS   = -static -I$(LIBDVR_INER_INCLUDE)
MY_LIBS   := -L$(LIBDVR_DEST_DIR)  -lhal -lpthread

PROGRAM  = network_test

include $(LIBDVR_BUILD_DIR)/Rules.mak


