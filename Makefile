# Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
# This program is free software. It comes without any warranty, to the extent
# permitted by applicable law. You can redistribute it and/or modify it under
# the terms of the Do What The Fuck You Want To Public License, Version 2, as
# published by Sam Hocevar. See the COPYING file for more details.

LDLIBS = $(shell pkg-config --libs $(LIBRARIES)) -lpthread -lrt
CFLAGS = $(shell pkg-config --cflags $(LIBRARIES)) -std=c99 -g -Wall -D_GNU_SOURCE
CFLAGS += $(addprefix -I,$(INCLUDES))

INCLUDES = include src src/modem
LIBRARIES = check glib-2.0 

TEST_ENABLE ?= false
PROFILE ?= false

ifeq ($(TEST_ENABLE),true)

RUBY ?= ruby

# Configure sources for test
CFLAGS += -DTEST 

# Enable gcov profiling to find out what test-coverage is
# Profiling is only available when test is enabled
ifeq ($(PROFILE),true)
CFLAGS += -fprofile-arcs -ftest-coverage
LDLIBS += -fprofile-arcs -ftest-coverage
GCOV_PREFIX =
GCOV_PREFIX_STRIP =
endif

# Mocking library
CMOCK = lib/cmock
CMOCK_SRC = $(wildcard $(CMOCK)/src/*.c)
CMOCK_OBJ = $(CMOCK_SRC:.c=.o)
INCLUDES += $(CMOCK)/src

# Modules to be mocked (need provide header file)
MOCK = include/attentive/parser.h include/attentive/cellular.h \
	   include/attentive/at.h include/attentive/at-unix.h \
	   src/modem/common.h

MOCK_SRC = $(addprefix $(TESTDEF)/mock/mock_,$(notdir $(MOCK:.h=.c)))
MOCK_OBJ = $(MOCK_SRC:.c=.o)
MOCK_GEN = CMOCK_DIR=./$(CMOCK) \
		   MOCK_OUT=./$(TESTDEF)/mock \
		   MOCK_PREFIX='mock_' \
		   $(RUBY) $(CMOCK)/scripts/create_mock.rb

# mock ups have to be in library archive, because of lining dependencies
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
RUNNER_OBJ = $(RUNNER_SRC:.c=.o)
RUNNER_GEN = $(RUBY) $(UNITY)/auto/generate_test_runner.rb

INCLUDES += include/attentive
else
# disable assertion in production build
CFLAGS += -DNDEBUG
endif

SRCS = $(wildcard src/*.c) $(wildcard src/modem/*c)  $(MOCK_SRC) $(TESTDEF_SRC) \
	   $(RUNNER_SRC) $(CMOCK_SRC) $(UNITY_SRC)


all: example test 
	@echo "+++ All good."""

example: src/example-at src/example-sim800


ifeq ($(TEST_ENABLE),true)
mocklib: $(MOCK_LIB)
test:  $(RUNNER)
	$(foreach x,$^,./$(x);)
else
test: tests/test-parser
	@echo "+++ Running parser test suite."
	tests/test-parser
endif

src/example-at: src/example-at.o src/parser.o src/at-unix.o
src/example-sim800: src/example-sim800.o src/modem/sim800.o src/modem/common.o src/cellular.o src/at-unix.o src/parser.o
tests/test-parser: tests/test-parser.o src/parser.o

ifeq ($(TEST_ENABLE),true)
$(RUNNER_OBJ): $(MOCK_OBJ)
$(MOCK_OBJ): $(MOCK_SRC)
$(MOCK_LIB): $(MOCK_OBJ)
	$(AR) rcs $@ $^

$(RUNNER): tests/%-runner: tests/%-runner.o tests/%-test.o src/%.o $(UNITY)/src/unity.o $(CMOCK_OBJ) $(MOCK_LIB)
	@echo "Linkink test suite runner"
	$(CC) $^ $(LDLIBS) -o $@

tests/mock/mock_%.c: include/attentive/%.h
	@echo "Generating mockups '$(notdir $<)': '$(notdir $@)'"
	@mkdir -p $(dir $@)
	@$(MOCK_GEN) $< >/dev/null

tests/mock/mock_%.c: src/modem/%.h 
	@echo "Generating mockups '$(notdir $<)': '$(notdir $@)'"
	@mkdir -p $(dir $@)
	@$(MOCK_GEN) $< >/dev/null

$(RUNNER_SRC): %-runner.c: %-test.c
	@echo "Generating test suite runner '$(notdir $@)'"
	@$(RUNNER_GEN) $< $@

%.d: %.c $(MOCK_LIB) Makefile.deps
	@echo "Generating dependency '$(notdir $@)'"
	$(CC) $(CFLAGS) -MM $< -MF $@

endif

ifneq (clean,$(MAKECMDGOALS))
-include Makefile.deps
endif

Makefile.deps: $(SRCS) 
		@$(CC) $(CFLAGS) -MM $^ >> $@

clean:
	$(RM) src/example-at src/example-sim800
	$(RM) src/*.o src/modem/*.o tests/*.o
	$(RM) src/*.gcno src/modem/*.gcno tests/*.gcno
	$(RM) src/*.gcda src/modem/*.gcda tests/*.gcda
	$(RM) -r tests/mock
	$(RM) tests/*runner* tests/modem/*runner*
	$(RM) $(MOCK_LIB)

.PHONY: all test mocklib clean
