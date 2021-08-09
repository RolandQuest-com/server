#pragma once

#include "defines.h"
#include "buffer.h"
#include "header.h"
#include "http_const.h"

typedef enum request_state {
    FRESH,
    INCOMPLETE,
    COMPLETE,
    MALFORMED
} request_state;

typedef struct request_packet {
    const char* http_method;
    char request_path[MAX_REQUEST_PATH_LENGTH + 1];
    const char* http_version;
    header_group headers;
    char* content;
    
    // Might need to make a struct to house information about
    // where in the content we are reading.
    request_state pstate;
} request_packet;

request_state request_parse(char_buffer* cbuf, request_packet* packet);
