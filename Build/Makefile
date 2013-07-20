############################################################################
#
#  for libdvr
#
# (C) Copyright 1992 to 2005 ZHEJIANG DaHua Technology Stock CO., LTD.
#
# sh4-linux version 1.0
#
############################################################################

PWD = $(shell pwd)
LIBDVR_BASE_DIR     = $(PWD)/..
LIBDVR_BUILD_DIR    = $(LIBDVR_BASE_DIR)/Build
include $(LIBDVR_BUILD_DIR)/config.mk
include $(LIBDVR_BUILD_DIR)/platform.mk

all:
	make -C ../Src
test:
	make -C ../Test
install:
	make -C ../Src install
clean:
	make -C ../Src clean


