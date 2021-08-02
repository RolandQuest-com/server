#pragma once

#include "defines.h"

#define MAX_HEADER_LENGTH 255
#define MAX_HEADER_COUNT 32

typedef struct header {
    char head[MAX_HEADER_LENGTH + 1];
    char body[MAX_HEADER_LENGTH + 1];
    u32 head_len;
    u32 body_len;
} header;

typedef struct header_group {
    header headers[MAX_HEADER_COUNT];
    u32 count;
} header_group;

u32 header_find(header_group* group, char* head);
u32 header_len(header_group* group);

