/*
 * Copyright © 2016 Artur Mądrzak <artur@madrzak.eu>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef SIM800_PRIVATE_H
#define SIM800_PRIVATE_H

/* 
 * THIS HEADER IS INTEND TO INCLUDE ONLY IN MODULE SOURCE FILE AND TESTS
 * DO NOT INCLUDE IT IN PRODUCTION CODE SPACE
 */

#define SIM800_AUTOBAUD_ATTEMPTS 5
#define SIM800_WAITACK_TIMEOUT 60
#define SIM800_FTP_TIMEOUT 60

enum sim800_socket_status {
    SIM800_SOCKET_STATUS_ERROR = -1,
    SIM800_SOCKET_STATUS_UNKNOWN = 0,
    SIM800_SOCKET_STATUS_CONNECTED = 1,
};

#define SIM800_NSOCKETS                 6
#define SIM800_CONNECT_TIMEOUT          60
#define SIM800_CIPCFG_RETRIES           10

struct cellular_sim800 {
    struct cellular dev;

    int ftpget1_status;
    enum sim800_socket_status socket_status[SIM800_NSOCKETS];
};

#endif

