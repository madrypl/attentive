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

void test_cellular_pdp_request_MaxFailuresReached(void)
{
    given_modem.pdp_failures = 3;
    given_modem.pdp_threshold = 3;
    
    TEST_FAIL();
    TEST_ASSERT_EQUAL(given_modem.pdp_failures);
}

void test_cellular_pdp_request_PdpOpenFail(void)
{
    TEST_FAIL();
}

void test_cellular_pdp_request_PdpOpenSuccess(void)
{
    TEST_FAIL();
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

void test_cellular_op_imei_ShortBuffer(void)
{
    TEST_FAIL();
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
