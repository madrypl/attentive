/*
 * Copyright © 2016 Artur Mądrzak <artur@madrzak.eu>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#include <string.h>
#include <unity.h>

#include "attentive/cellular.h"
#include "attentive/at.h"
#include "mock_common.h"

#define EXPECTED_APN "internet.apn.string"
#define DUMMY_APN "internet.apn.string"

static int expected_action(struct cellular *modem);
static int expected_fail_action(struct cellular *modem);
static int unexpected_action(struct cellular *modem);

static struct cellular_ops expected_success_ops = {
    .attach = expected_action,
    .detach = expected_action
};

static struct cellular_ops expected_fail_ops = {
    .attach = expected_fail_action,
    .detach = expected_fail_action
};


static struct cellular_ops unexpected_ops = {
    .attach = unexpected_action,
    .detach = unexpected_action
};

static struct cellular_ops no_ops = {
    .attach = NULL,
    .detach = NULL
};

static struct cellular dummy_modem;
static struct at expected_at;
static struct at dummy_at;

static int expected_action(struct cellular *modem) { return 0; }
static int expected_fail_action(struct cellular *modem) { return -1; }
static int unexpected_action(struct cellular *modem)
{
    TEST_FAIL();
    return -1; /* just for avoid compiler warning */
}

void setUp(void)
{
    memset(&dummy_modem, 0, sizeof(dummy_modem));
    memset(&expected_at, 0, sizeof(expected_at));
}

void tearDown(void)
{
}


void test_cellular_attach_AlreadyAttached(void)
{
    dummy_modem.at = &expected_at;
    dummy_modem.ops = &unexpected_ops;
    
    TEST_ASSERT_EQUAL(0, cellular_attach(&dummy_modem, &dummy_at, EXPECTED_APN));
    TEST_ASSERT_EQUAL(NULL, dummy_modem.apn);
    TEST_ASSERT_EQUAL(&expected_at, dummy_modem.at);
}

void test_cellular_attach_AttachableModem(void)
{
    dummy_modem.ops = &expected_success_ops;
    cellular_pdp_success_Expect(&dummy_modem);

    TEST_ASSERT_EQUAL(0, cellular_attach(&dummy_modem, &expected_at, EXPECTED_APN));
    TEST_ASSERT_EQUAL(EXPECTED_APN, dummy_modem.apn);
    TEST_ASSERT_EQUAL(&expected_at, dummy_modem.at);
}

void test_cellular_attach_NotAttachableModem(void)
{
    dummy_modem.ops = &no_ops;
    cellular_pdp_success_Expect(&dummy_modem);

    TEST_ASSERT_EQUAL(0, cellular_attach(&dummy_modem, &expected_at, EXPECTED_APN));
    TEST_ASSERT_EQUAL(EXPECTED_APN, dummy_modem.apn);
    TEST_ASSERT_EQUAL(&expected_at, dummy_modem.at);
}

void test_cellular_detach_AlreadyDetached(void)
{
    dummy_modem.apn = EXPECTED_APN;
    dummy_modem.ops = &unexpected_ops;

    TEST_ASSERT_EQUAL(0, cellular_detach(&dummy_modem));
    TEST_ASSERT_EQUAL(EXPECTED_APN, dummy_modem.apn);
    TEST_ASSERT_EQUAL(NULL, dummy_modem.at);
}

void test_cellular_detach_DetachableModemSuccess(void)
{
    dummy_modem.at = &dummy_at;
    dummy_modem.apn = DUMMY_APN;
    dummy_modem.ops = &expected_success_ops;

    TEST_ASSERT_EQUAL(0, cellular_detach(&dummy_modem));
    TEST_ASSERT_EQUAL(NULL, dummy_modem.apn);
    TEST_ASSERT_EQUAL(NULL, dummy_modem.at);
}

void test_cellular_detach_DetachableModemFail(void)
{
    dummy_modem.at = &dummy_at;
    dummy_modem.apn = DUMMY_APN;
    dummy_modem.ops = &expected_fail_ops;

    TEST_ASSERT_NOT_EQUAL(0, cellular_detach(&dummy_modem));
    TEST_ASSERT_EQUAL(NULL, dummy_modem.apn);
    TEST_ASSERT_EQUAL(NULL, dummy_modem.at);
}

void test_cellular_detach_NotDetachableModem(void)
{
    dummy_modem.at = &dummy_at;
    dummy_modem.apn = DUMMY_APN;
    dummy_modem.ops = &no_ops;
    
    TEST_ASSERT_EQUAL(0, cellular_detach(&dummy_modem));
    TEST_ASSERT_EQUAL(NULL, dummy_modem.apn);
    TEST_ASSERT_EQUAL(NULL, dummy_modem.at);
}

/* vim: set ts=4 sw=4 et: */
