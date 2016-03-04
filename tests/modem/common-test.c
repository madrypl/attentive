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
#include "mock_at.h"

static int expected_action(struct cellular *modem);
static int expected_fail_action(struct cellular *modem);
static int unexpected_action(struct cellular *modem);

static struct cellular_ops ops;
static struct cellular given_modem;
static struct at given_at;

static int expected_action(struct cellular *modem) { return 0; }
static int expected_fail_action(struct cellular *modem) { return -1; }
static int unexpected_action(struct cellular *modem)
{
    TEST_FAIL();
    return -1; /* just for avoid compiler warning */
}

void setUp(void)
{
    memset(&ops, 0, sizeof(ops));
    memset(&given_modem, 0, sizeof(given_modem));
    memset(&given_at, 0, sizeof(given_at));
    given_modem.at = &given_at;
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
    char imei[17];
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CGSN", NULL); 
    TEST_ASSERT_EQUAL(-1, cellular_op_imei(&given_modem, imei, sizeof(imei)));
}

void test_cellular_op_imei_ResponseLonger(void)
{
    char imei[17];
    const char to_long_response[] = "012345678901234567";
    at_set_timeout_Expect(&given_at, 1);

    at_command_ExpectAndReturn(&given_at, "AT+CGSN", to_long_response);
    TEST_ASSERT_EQUAL(0, cellular_op_imei(&given_modem, imei, sizeof(imei)));   
    TEST_ASSERT_EQUAL_STRING("0123456789012345", imei);
}

void test_cellular_op_iccid_FormatBufferToShort(void)
{
    TEST_ASSERT_EQUAL(-1, cellular_op_iccid(&given_modem, NULL, 123456780L));
    TEST_ASSERT_EQUAL(ENOSPC, errno);
}

void test_cellular_op_iccid_NullResponse(void)
{
    char iccid[20];
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CCID", NULL); 
    TEST_ASSERT_EQUAL(-1, cellular_op_iccid(&given_modem, iccid, sizeof(iccid)));
}

void test_cellular_op_iccid_ResponseLonger(void)
{
    char iccid[21];
    const char to_long_response[] = "0123456789012345678901";
    at_set_timeout_Expect(&given_at, 1);

    at_command_ExpectAndReturn(&given_at, "AT+CCID", to_long_response);
    TEST_ASSERT_EQUAL(0, cellular_op_iccid(&given_modem, iccid, sizeof(iccid)));   
    TEST_ASSERT_EQUAL_STRING("01234567890123456789", iccid);
}

void test_cellular_op_creg_Valid(void)
{
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CREG?", "+CREG: 1,2");
    TEST_ASSERT_EQUAL(2, cellular_op_creg(&given_modem));
}

void test_cellular_op_creg_InvalidResponse(void)
{
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CREG?", "+CREG: f,2");
    TEST_ASSERT_EQUAL(-1, cellular_op_creg(&given_modem));
}

void test_cellular_op_rssi_Valid(void)
{
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CSQ", "+CSQ: 50,1");
    TEST_ASSERT_EQUAL(50, cellular_op_rssi(&given_modem));
}

void test_cellular_op_rssi_InvalidResponse(void)
{
    at_set_timeout_Expect(&given_at, 1);
    at_command_ExpectAndReturn(&given_at, "AT+CSQ", "+CSQ: f,2");
    TEST_ASSERT_EQUAL(-1, cellular_op_rssi(&given_modem));
}

void test_reminder(void)
{
    TEST_FAIL_MESSAGE("Implement rest tests");
}
