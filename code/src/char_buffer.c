#include "char_buffer.h"

char_buffer sb_create(char* buf, u32 size, u32 len){
    char_buffer ret = {
        .buffer = buf,
        .buffer_size = size,
        .data_len = len,
        .data_pos = 0
    };
    return ret;
}
u32 sb_len(char_buffer* buf){
    return buf->data_len;
}
char sb_get(char_buffer* buf){
    return buf->buffer[buf->data_pos++];
}
char_buffer sb_get_delim(char_buffer* buf, char delim){
    //WARNING: The buffer returned will share the memory space of the 'buf' parameter.
    char_buffer ret = {
        .buffer = buf->buffer + buf->data_pos,
        .buffer_size = 0,
        .data_len = 0,
        .data_pos = 0
    };
    
    u32 len = 0;
    while(buf->data_pos < buf->data_len){
        if(sb_get(buf) == delim){
            break;
        }
        len++;
    }
    
    ret.data_len = len;
    return ret;
}
char sb_peek(char_buffer* buf){
    //TODO: Putting in bounds checking.
    return buf->buffer[buf->data_pos];
}
void sb_seek(char_buffer* buf, u32 spos){
    if(spos > buf->data_len - 1){
        spos = buf->data_len - 1;
    }
    buf->data_pos = spos;
}
