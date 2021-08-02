#pragma once

#include "header.h"

extern const char* const HTTP_STATUS_OK;

typedef struct return_packet {
    const char* http_status;
    header_group headers;
    char* content;
} return_packet;

u32 response_len(return_packet* packet);
char* response_tos(return_packet* packet);

