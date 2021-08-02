#include "request_packet.h"

#include "string.h"

const char* const HTTP_VERSION_1_0 = "HTTP/1.0";
const char* const HTTP_VERSION_1_1 = "HTTP/1.1";
const char* const HTTP_VERSION_2_0 = "HTTP/2.0";

const char* const HTTP_METHOD_GET = "GET";

u32 request_len(request_packet* packet){
    u32 len = 0;
    len += strlen(packet->http_method) + 1;
    len += strlen(packet->request_path) + 1;
    len += strlen(packet->http_version) + 2;
    len += header_len(&packet->headers) + 2;
    return len + strlen(packet->content);
}

char* request_tos(request_packet* packet){
    //TODO: Is this function useful?
    return 0;
}

void request_parse(request_packet* packet, request_state* state){
    
    
    
}