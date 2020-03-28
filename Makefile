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
DRIVER_INCD := $(DRIVERD)/include
DRIVER_SRCD := $(DRIVERD)/src

ALL_LIB_SRCF := $(shell find $(LIB_SRCD) -type f -name *.c)
ALL_LIB_OBJF := $(patsubst $(LIB_SRCD)/%,$(LIB_BLDD)/%,$(ALL_LIB_SRCF:.c=.o))
LIB_INC = -I $(LIB_INCD)
LIB := lib$(LIB_NAME).a

ALL_DRIVER_SRCF := $(shell find $(DRIVER_SRCD) -type f -name *.c)
ALL_DRIVER_OBJF := $(patsubst $(DRIVER_SRCD)/%,$(DRIVER_BLDD)/%,$(ALL_DRIVER_SRCF:.c=.o))
ALL_DRIVER_BIN := $(patsubst $(DRIVER_BLDD)/%,$(DRIVER_BIND)/%,$(ALL_DRIVER_OBJF:.o=.out))
DRIVER_INC = -I $(DRIVER_INCD)

ALL_TESTS := $(shell find $(TESTD) -type f -name *.sh)

CFLAGS := -Wall -Werror -MD
LDFLAGS := -l$(LIB_NAME)
STD := -std=gnu11

CFLAGS += $(STD)

.PHONY: clean lib all test

all: lib driver

lib: lib_setup $(LIB)

lib_setup: $(LIB_BLDD) 

$(LIB_BLDD):
	mkdir -p $(LIB_BLDD)

$(LIB): $(ALL_LIB_OBJF)
	ar -rcs $(LIBD)/$@ $^

$(LIB_BLDD)/%.o: $(LIB_SRCD)/%.c
	$(CC) $(CFLAGS) $(LIB_INC) -c -o $@ $<

driver: driver_setup $(ALL_DRIVER_BIN)

driver_setup: $(DRIVER_BLDD) $(DRIVER_BIND)

$(DRIVER_BLDD):
	mkdir -p $(DRIVER_BLDD)

$(DRIVER_BIND):
	mkdir -p $(DRIVER_BIND)

$(DRIVER_BIND)/%.out: $(DRIVER_BLDD)/%.o
	$(CC) $< -o $@ -L$(LIBD) $(LDFLAGS) 

$(DRIVER_BLDD)/%.o: $(DRIVER_SRCD)/%.c
	$(CC) $(CFLAGS) $(LIB_INC) $(DRIVER_INC) -c -o $@ $<

clean: lib_clean driver_clean

lib_clean:
	$(RM) -r $(LIB_BLDD) $(LIBD)/$(LIB)

driver_clean:
	$(RM) -r $(DRIVER_BLDD) $(DRIVER_BIND)

test: all $(ALL_TESTS)
	for i in $(ALL_TESTS); do \
		/bin/bash $$i; done

.PRECIOUS: $(LIB_BLDD)/*.d $(DRIVER_BLDD)/*.d
-include $(LIB_BLDD)/*.d $(DRIVER_BLDD)/*.d