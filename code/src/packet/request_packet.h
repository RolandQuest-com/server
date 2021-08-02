#pragma once

#include "header.h"

#define MAX_REQUEST_PATH_LENGTH 128

extern const char* const HTTP_VERSION_1_0;
extern const char* const HTTP_VERSION_1_1;
extern const char* const HTTP_VERSION_2_0;

extern const char* const HTTP_METHOD_GET;

typedef struct request_packet {
    const char* http_method;
    char request_path[MAX_REQUEST_PATH_LENGTH];
    const char* http_version;
    header_group headers;
    char* content;
} request_packet;





/*
bool is_complete;
bool chunked;
RP_SECTION section; // METHOD, PATH, VERSION, HEADERS, CONTENT
char* leftover;
*/
typedef struct chunked_state {
    u32 temp;
} chunked_state;

typedef union request_state {
    u32 temp;
} request_state;






void request_parse(request_packet* packet, request_state* state);
u32 request_len(request_packet* packet);
char* request_tos(request_packet* packet);

