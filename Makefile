# Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
# This program is free software. It comes without any warranty, to the extent
# permitted by applicable law. You can redistribute it and/or modify it under
# the terms of the Do What The Fuck You Want To Public License, Version 2, as
# published by Sam Hocevar. See the COPYING file for more details.

CFLAGS = $(shell pkg-config --cflags $(LIBRARIES)) -std=c99 -g -Wall -Iinclude -D_GNU_SOURCE
LDLIBS = $(shell pkg-config --libs $(LIBRARIES)) -lpthread -lrt

INCLUDES = 
LIBRARIES = check glib-2.0 

TEST_ENABLE ?= false

ifeq ($(TEST_ENABLE),true)
RUBY = ~/usr/bin/ruby
LDLIBS += -l$(MOCK_LIB)
# Mocking library
CMOCK = lib/cmock
CMOCK_SRC = $(wildcard $(CMOCK)/src/*.c)
CMOCK_OBJ = $(CMOCK_SRC:.c=.o)
INCLUDES += $(CMOCK)/src

# Modules to be mocked (need provide header file)
MOCK = include/attentive/parser.h
MOCK_SRC = $(addprefix $(TESTDEF)/mock/mock_,$(notdir $(MOCK:.h=.c)))
MOCK_OBJ = $(MOCK_SRC:.c=.o)
MOCK_GEN = MOCK_OUT=./$(TESTDEF)/mock \
		   CMOCK_DIR=./$(CMOCK) \
		   MOCK_PREFIX='mock_' \
		   $(RUBY) $(CMOCK)/scripts/create_mock.rb
MOCK_LIB = libmock.a
INCLUDES += $(TESTDEF)/mock

# Test framework
# Unity
UNITY = lib/unity
UNITY_SRC = $(wildcard $(UNITY)/src/*.c)
UNITY_OBJ = $(UNITY_SRC:.c=.o)
INCLUDES += $(UNITY)/src

# Test definitions for module
TESTDEF = tests
TESTDEF_SRC = $(wildcard $(TESTDEF)/*-test.c) $(wildcard $(TESTDEF)/modem/*-test.c)
TESTDEF_OBJ = $(TESTDEF_SRC:.c=.o)

# RUNNER = RUNNER.c + TESTDEF + PROJFILE + libmock.a
RUNNER = $(patsubst %-test.c,%-runner,$(TESTDEF_SRC))
RUNNER_SRC = $(addsuffix .c,$(RUNNER))
RUNNER_GEN = $(RUBY) $(UNITY)/auto/generate_test_runner.rb

# TODO: get rid of it... try to cmock include <attentive/parser.h> instead <parser.h>
# TODO: need to handle anonymous structs in testing
INCLUDES += include/attentive
CFLAGS += $(addprefix -I,$(INCLUDES))
endif

all: example test 
	@echo "+++ All good."""

example: src/example-at src/example-sim800

ifeq ($(TEST_ENABLE),true)
test: $(RUNNER)
	$(foreach x,$^,./x;)
else
test: tests/test-parser
	@echo "+++ Running parser test suite."
	tests/test-parser
endif

clean:
	$(RM) src/example-at src/example-sim800
	$(RM) src/*.o src/modem/*.o tests/*.o
ifeq ($(TEST_ENABLE),true)
	$(RM) -r tests/mock
	$(RM) tests/*runner.c tests/*runner
endif

PARSER = include/attentive/parser.h
AT = include/attentive/at.h include/attentive/at-unix.h $(PARSER)
CELLULAR = include/attentive/cellular.h $(AT)
MODEM = src/modem/common.h $(CELLULAR)

src/parser.o: src/parser.c $(PARSER)
src/at-unix.o: src/at-unix.c $(AT)
src/cellular.o: src/cellular.c $(CELLULAR)
src/modem/common.o: src/modem/common.c $(MODEM)
src/modem/generic.o: src/modem/generic.c $(MODEM)
src/modem/sim800.o: src/modem/sim800.c $(MODEM)
src/modem/telit2.o: src/modem/telit2.c $(MODEM)
src/example-at.o: src/example-at.c $(AT)
src/example-sim800.o: src/example-sim800.c $(CELLULAR)
src/test-parser.o: tests/test-parser.o $(MODEM)
src/example-at: src/example-at.o src/parser.o src/at-unix.o
src/example-sim800: src/example-sim800.o src/modem/sim800.o src/modem/common.o src/cellular.o src/at-unix.o src/parser.o
tests/test-parser: tests/test-parser.o src/parser.o

ifeq ($(TEST_ENABLE),true)

$(MOCK_OBJ): $(MOCK_SRC)
$(MOCK_LIB): $(MOCK_OBJ)

tests/%-runner: tests/%-runner.o tests/%-test.o src/%.o $(UNITY)/src/unity.o
	@echo "Linkink test suite runner"
	$(CC) $^ $(LDFLAGS) -o $@

tests/mock/mock-%.c: include/attentive/%.h
	@echo "Generating mockups '$(notdir $<)': '$(notdir $@)'"
	@mkdir -p $(dir $@)
	@$(MOCK_GEN) $< >/dev/null

%-runner.c: %-test.c
	@echo "Generating test suite runner '$(notdir $<)': '$(notdir $@)'"
	@$(RUNNER_GEN) $< $@

endif

.PHONY: all test clean
