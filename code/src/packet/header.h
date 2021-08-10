#pragma once

#include "defines.h"
#include "http_const.h"

extern const char* const HTTP_HEADER_CONTENT_LENGTH;

typedef struct http_header {
    char head[MAX_HEADER_LENGTH + 1];
    char body[MAX_HEADER_LENGTH + 1];
} http_header;

typedef struct header_group {
    http_header headers[MAX_HEADER_COUNT];
    u32 count;
} header_group;

u32 header_find(header_group* group, const char* const head);
const char* header_val_s(header_group* group, const char* const head);
const char* header_val_u(header_group* group, u32 head);
//u32 header_len(header_group* group);

