#pragma once

#include "header.h"
#include "http_const.h"

typedef struct response_packet {
    const char* http_status;
    header_group headers;
    char* content;
} response_packet;
