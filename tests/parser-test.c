/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * Copyright © 2016 Artur Mądrzak <artur@madrzak.eu>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#include <string.h>
#include <unity.h>
#include "parser.h"

void handle_response(const char *line, size_t len, void *priv);
void handle_urc(const char *line, size_t len, void *priv);

struct at_parser *parser = NULL;
struct at_parser_callbacks cbs = {
        .handle_response = handle_response,
        .handle_urc = handle_urc,
    };

void handle_response(const char *line, size_t len, void *priv)
{
}

void handle_urc(const char *line, size_t len, void *priv)
{
}

void setUp(void)
{
    parser = at_parser_alloc(&cbs, 256, NULL);
}

void tearDown(void)
{
    at_parser_free(parser);
}

void test_parser_alloc(void)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(parser, "at_parser_alloc returned null");
    TEST_ASSERT_NULL(parser->priv);
    TEST_ASSERT_NOT_NULL(parser->buf);
    TEST_ASSERT_EQUAL(256, parser->buf_size);
    TEST_ASSERT_EQUAL(&cbs, parser->cbs);
}

void test_parser_reset(void)
{
    /* set some random data to structure (do not overwrite locally allocated buf) */
    memset(parser, 0xaa, sizeof(struct at_parser) - sizeof(char*));
    at_parser_reset(parser);
    TEST_ASSERT_EQUAL(STATE_IDLE, parser->state);
    TEST_ASSERT_EQUAL(false, parser->expect_dataprompt);
    TEST_ASSERT_EQUAL(0, parser->buf_used);
    TEST_ASSERT_EQUAL(0, parser->buf_current);
    TEST_ASSERT_EQUAL(0, parser->data_left);
}

void test_parser_expect_dataprompt(void)
{
    parser->expect_dataprompt = false;
    at_parser_expect_dataprompt(parser);
    TEST_ASSERT_EQUAL(true, parser->expect_dataprompt);
}

void test_parser_await_response_DataPrompt(void)
{
    parser->expect_dataprompt = true;
    at_parser_await_response(parser);
    TEST_ASSERT_EQUAL(STATE_DATAPROMPT, parser->state);
}

void test_parser_await_response_NormalLine(void)
{
    parser->expect_dataprompt = false;
    at_parser_await_response(parser);
    TEST_ASSERT_EQUAL(STATE_READLINE, parser->state);
}

static const char *const test_table[] = 
{
    "wakawaka",
    "some",
    "waka",
    "blom",
    NULL,
};

void test_prefix_in_table_IsInTable(void)
{
    TEST_ASSERT_TRUE(at_prefix_in_table("waka", test_table));
}

void test_prefix_in_table_IsNotInTable(void)
{
    TEST_ASSERT_FALSE(at_prefix_in_table("winki", test_table));
}

/* TODO: generic_line_scanner tests */

void test_parser_append_Empty(void)
{
    
}

void test_parser_append_PartiallyFilled(void)
{
}

void test_parser_append_Full(void)
{
}

void test_parser_include_line_AppendSuccess(void)
{
}

void test_parser_include_line_AppendOverflow(void)
{
}

void test_parser_finalize_WithNewLine(void)
{
}

void test_parser_finalize_WithNoNewLine(void)
{
}

void test_parser_handle_line_EmptyLine(void)
{
}

void test_parser_hadnle_line_NoCustomScanLine(void)
{
}

void test_parser_handle_line_CustomScanLineReturnUnknownResponse(void)
{
}

void test_parser_handle_line_CustomScanLineReturnKnownResponse(void)
{
}

/* Functional tests, see test-parser.c */

