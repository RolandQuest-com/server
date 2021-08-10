#include "header.h"

#include "string.h"

const char* const HTTP_HEADER_CONTENT_LENGTH = "content-length";

u32 header_find(header_group* group, char* head){
    for(int i = 0; i < group->count; i++){
        if(strcmp(group->headers[i].head, head) == 0){
            return i;
        }
    }
    return -1;
}

const char*  header_val_s(header_group* group, char* head){
    u32 i = header_find(group, head);
    if (i == -1){
        return NULL;
    }
    return header_val_u(group, i);
}

const char*  header_val_u(header_group* group, u32 head){
    if(head > group->count - 1){
        return NULL;
    }
    return group->headers[head].body;
}

/*
u32 header_len(header_group* group){
    u32 len = 0;
    for(u32 i = 0; i < group->count; i++){
        len += strlen(group->headers[i].head);
        len += 2; // ": "
        len += strlen(group->headers[i].body);
        len += 2; // \r\n
    }
    return len;
}
*/
