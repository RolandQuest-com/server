#include "header.h"

#include "string.h"

u32 header_find(header_group* group, char* head){
    
    for(int i = 0; i < group->count; i++){
        if(strcmp(group->headers[i].head, head) == 0){
            return i;
        }
    }
    
    return -1;
}

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

