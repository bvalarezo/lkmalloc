CC := gcc
DRIVERD := driver
LIBD := lkmalloc
TESTD := tests

LIB_SRCD := $(LIBD)/src
LIB_BLDD := $(LIBD)/build
LIB_INCD := $(LIBD)/include

DRIVER_SRCD := $(DRIVERD)/src
DRIVER_BIND := $(DRIVERD)/bin

ALL_LIB_SRCF := $(shell find $(LIB_SRCD) -type -f -name *.c)
ALL_LIB_OBJF := $(shell find $(LIB_BLDD) -type -f -name *.o)
LIB_INC = -I $(LIB_INCD)

CFLAGS := -g -Wall -Werror
STD := -std=gnu11

CFLAGS += $(STD)

setup: 