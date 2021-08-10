#pragma once

#include "defines.h"

typedef struct char_buffer {
    char* buffer;
    u32 buffer_size;
    u32 data_len;
    u32 data_pos;
} char_buffer;

char_buffer sb_create(char* buf, u32 size, u32 len);
u32 sb_len(char_buffer* buf);
char sb_get(char_buffer* buf);
char_buffer sb_get_delim(char_buffer* buf, char delim);
char sb_peek(char_buffer* buf);
void sb_seek(char_buffer* buf, u32 spos);
