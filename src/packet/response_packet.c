#include "response_packet.h"

#include "string.h"

const char* const HTTP_STATUS_OK = "200 OK";

u32 response_len(return_packet* packet){
    u32 len = 0;
    len += strlen(packet->http_status) + 2;
    len += header_len(&packet->headers) + 2;
    return len + strlen(packet->content);
}

char* response_tos(return_packet* packet){
   return 0; 
}

