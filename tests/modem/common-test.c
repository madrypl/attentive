/*
 * Copyright © 2016 Artur Mądrzak <artur@madrzak.eu>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#include <string.h>
#include <unity.h>

#include "common.h"
#include "mock_at-unix.h"

static int expected_action(struct cellular *modem);
static int expected_fail_action(struct cellular *modem);
static int unexpected_action(struct cellular *modem);

static struct cellular_ops ops;
static struct cellular given_modem;

static int expected_action(struct cellular *modem) { return 0; }
static int expected_fail_action(struct cellular *modem) { return -1; }
static int unexpected_action(struct cellular *modem)
{
    TEST_FAIL();
    return -1; /* just for avoid compiler warning */
}

void setUp(void)
{
    memset(&given_modem, 0, sizeof(given_modem));
}

void tearDown(void)
{
}

void test_cellular_pdp_request_UseUninitalized(void)
{
    given_modem.ops = &ops;
    ops.pdp_open = unexpected_action;
    ops.pdp_close = unexpected_action;
    TEST_ASSERT_EQUAL(-1, cellular_pdp_request(&given_modem));
}

void test_cellular_pdp_request_MaxFailuresReachedAndPdpOpenSuccess(void)
{
    given_modem.pdp_failures = 3;
    given_modem.pdp_threshold = 3;
    given_modem.ops = &ops;
    ops.pdp_open = expected_action;
    ops.pdp_close = expected_action;
    TEST_ASSERT_EQUAL(0, cellular_pdp_request(&given_modem));
    TEST_ASSERT_EQUAL(9, given_modem.pdp_threshold);
    TEST_ASSERT_EQUAL(3, given_modem.pdp_failures);
}

void test_cellular_pdp_request_PdpOpenFail(void)
{
    given_modem.pdp_threshold = 1;
    given_modem.ops = &ops;
    ops.pdp_open = expected_fail_action;
    TEST_ASSERT_EQUAL(-1, cellular_pdp_request(&given_modem));
    TEST_ASSERT_EQUAL(1, given_modem.pdp_failures);
}

void test_cellular_pdp_success_BothValues(void)
{
    given_modem.pdp_failures = 0x1234abcd;
    given_modem.pdp_threshold = 0x567890ab;

    cellular_pdp_success(&given_modem);
    TEST_ASSERT_EQUAL(0, given_modem.pdp_failures);
    TEST_ASSERT_EQUAL(3, given_modem.pdp_threshold);
}

void test_cellular_pdp_failure_CheckIncrementCorrectly(void)
{
    given_modem.pdp_failures = 18;
    cellular_pdp_failure(&given_modem);
    TEST_ASSERT_EQUAL(19, given_modem.pdp_failures);
}

void test_cellular_op_imei_FormatBufferToShort(void)
{
    TEST_ASSERT_EQUAL(-1, cellular_op_imei(&given_modem, NULL, 123456780L));
    TEST_ASSERT_EQUAL(ENOSPC, errno);
}

void test_cellular_op_imei_NullResponse(void)
{
    TEST_FAIL();
}

void test_cellular_op_imei_ResponseTooLong(void)
{
    TEST_FAIL();
}

void test_cellular_op_imei_ResponseNotMatchFormat(void)
{
    TEST_FAIL();
}

void test_reminder(void)
{
    TEST_FAIL_MESSAGE("Implement rest tests");
}
