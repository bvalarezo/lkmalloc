CC := gcc
DRIVERD := driver
LIBD := lib
TESTD := tests
LIB_NAME := lkmalloc

LIB_BLDD := $(LIBD)/build
LIB_INCD := $(LIBD)/include
LIB_SRCD := $(LIBD)/src

DRIVER_BIND := $(DRIVERD)/bin
DRIVER_BLDD := $(DRIVERD)/build
DRIVER_SRCD := $(DRIVERD)/src

ALL_LIB_SRCF := $(shell find $(LIB_SRCD) -type -f -name *.c)
ALL_LIB_OBJF := $(patsubst $(LIB_SRCD)/%,$(LIB_BLDD)/%,$(ALL_LIB_SRCF:.c=.o))
LIB_INC = -I $(LIB_INCD)
LIB := lib$(LIB_NAME).a

CFLAGS := -g -Wall -Werror
LDFLAGS := -l$(LIB_NAME)
STD := -std=gnu11

CFLAGS += $(STD)

setup: lib_setup driver_setup

lib_setup: $(LIB_BLDD) 

$(LIB_BLDD):
	mkdir -p $(LIB_BLDD)

driver_setup: $(DRIVER_BLDD) $(DRIVER_BIND)

$(DRIVER_BLDD):
	mkdir -p $(DRIVER_BLDD)

$(DRIVER_BIND):
	mkdir -p $(DRIVER_BIND)

$(LIB): $(ALL_LIB_OBJF)
	ar -rcs $(LIBD)/$@ $^

$(LIB_BLDD)/%.o: $(LIB_SRCD)/%.c
	$(CC) $(CFLAGS) $(LIB_INC) -c -o $@ $<









clean: lib_clean driver_clean

lib_clean:
	$(RM) -r $(LIB_BLDD)

driver_clean:
	$(RM) -r $(DRIVER_BLDD) $(DRIVER_BIND)