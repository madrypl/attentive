/*
 * Copyright © 2016 Artur Mądrzak <artur@madrzak.eu>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

/* 
 * THIS HEADER IS INTEND TO INCLUDE ONLY IN MODULE SOURCE FILE AND TESTS
 * DO NOT INCLUDE IT IN PRODUCTION CODE SPACE
 */

enum at_parser_state {
    STATE_IDLE,
    STATE_READLINE,
    STATE_DATAPROMPT,
    STATE_RAWDATA,
    STATE_HEXDATA,
};

struct at_parser {
    const struct at_parser_callbacks *cbs;
    void *priv;

    enum at_parser_state state;
    bool expect_dataprompt;
    size_t data_left;
    int nibble;

    char *buf;
    size_t buf_used;
    size_t buf_size;
    size_t buf_current;
};

#endif

