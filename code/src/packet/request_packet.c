#include "request_packet.h"

#include "stdlib.h"
#include "string.h"

const char* __find_method(char* buffer){
    if(strcmp(HTTP_METHOD_GET, buffer) == 0){
        return HTTP_METHOD_GET;
    }
    if(strcmp(HTTP_METHOD_POST, buffer) == 0){
        return HTTP_METHOD_POST;
    }
    if(strcmp(HTTP_METHOD_PUT, buffer) == 0){
        return HTTP_METHOD_PUT;
    }
    if(strcmp(HTTP_METHOD_HEAD, buffer) == 0){
        return HTTP_METHOD_HEAD;
    }
    if(strcmp(HTTP_METHOD_DELETE, buffer) == 0){
        return HTTP_METHOD_DELETE;
    }
    if(strcmp(HTTP_METHOD_PATCH, buffer) == 0){
        return HTTP_METHOD_PATCH;
    }
    if(strcmp(HTTP_METHOD_OPTIONS, buffer) == 0){
        return HTTP_METHOD_OPTIONS;
    }
    return NULL;
}
const char* __find_version(char* buffer){
    if(strcmp(HTTP_VERSION_1_0, buffer) == 0){
        return HTTP_VERSION_1_0;
    }
    if(strcmp(HTTP_VERSION_1_1, buffer) == 0){
        return HTTP_VERSION_1_1;
    }
    if(strcmp(HTTP_VERSION_2_0, buffer) == 0){
        return HTTP_VERSION_2_0;
    }
    return NULL;
}

// return request_state
request_state __extract_method_line(char_buffer* cbuf, request_packet* packet){
    
    // METHOD - PATH - VERSION
    char con[3][MAX_REQUEST_PATH_LENGTH + 1] = {{0}};
    
    // Method
    char_buffer m = sb_get_delim(cbuf, ' ');
    memcpy(con[0], m.buffer, m.data_len);
    
    // Path
    char_buffer p = sb_get_delim(cbuf, ' ');
    memcpy(con[1], p.buffer, p.data_len);
    
    // Version
    char_buffer v = sb_get_delim(cbuf, '\r');
    memcpy(con[2], v.buffer, v.data_len);
    
    sb_get(cbuf);
    
    const char* method = __find_method(con[0]);
    const char* version = __find_version(con[2]);
    
    if(method == NULL || version == NULL){
        return MALFORMED;
    }
    
    packet->http_method = method;
    packet->http_version = version;
    memcpy(packet->request_path, con[1], MAX_REQUEST_PATH_LENGTH);
    return COMPLETE;
}
request_state __extract_headers(char_buffer* cbuf, request_packet* packet){
    
    bool complete = false;
    u32 header_index = 0;
    
    while(!complete){
        
        char con[2][MAX_HEADER_LENGTH + 1] = {0};
        
        // Key
        char_buffer k = sb_get_delim(cbuf, ':');
        memcpy(con[0], k.buffer, k.data_len);
        sb_get(cbuf);
        
        // Value
        char_buffer v = sb_get_delim(cbuf, '\r');
        memcpy(con[1], v.buffer, v.data_len);
        sb_get(cbuf);
        
        memcpy(packet->headers.headers[header_index].head, con[0], MAX_HEADER_LENGTH);
        memcpy(packet->headers.headers[header_index].body, con[1], MAX_HEADER_LENGTH);
        packet->headers.count++;
        header_index++;
        
        // A carriage return here means we're done with headers.
        if(sb_peek(cbuf) == '\r'){
            sb_get(cbuf);
            sb_get(cbuf);
            complete = true;
        }
        if(header_index == MAX_HEADER_COUNT){
            complete = true;
        }
        
    }
    
    return COMPLETE;
}
request_state __extract_content(char_buffer* cbuf, request_packet* packet){
    //TODO: Implement chunked content.
    
    const char* value = header_val_s(&packet->headers, HTTP_HEADER_CONTENT_LENGTH);
    if(value == NULL){
        return MALFORMED;
    }
    char* temp;
    u64 content_length = strtoul(value, &temp, 10);
    if(content_length == 0){
        return MALFORMED;
    }
    
    //TODO: Potentially put limits on amount of data that can be sent.
    packet->content = malloc(content_length);
    
    u32 cbuf_remaining = cbuf->data_len - cbuf->data_pos;
    u32 to_copy = cbuf_remaining > content_length ? content_length : cbuf_remaining;
    memcpy(packet->content, cbuf->buffer + cbuf->data_pos, to_copy);
    return COMPLETE;
}

//NOTE: Will return MALFORMED if all headers are not provided.
//NOTE: Content data can be done piecemeal.
request_state request_parse(char_buffer* cbuf, request_packet* packet){
    
    if(packet->pstate == MALFORMED || packet->pstate == COMPLETE){
        return packet->pstate;
    }
    
    if(packet->pstate == FRESH){
        
        if(__extract_method_line(cbuf, packet) == MALFORMED){
            return MALFORMED;
        }
        if(__extract_headers(cbuf, packet) == MALFORMED){
            return MALFORMED;
        }
    }
    
    //TODO: What if not all content is sent in first read?
    if(packet->http_method == HTTP_METHOD_POST){
        if(__extract_content(cbuf, packet) == MALFORMED){
            return MALFORMED;
        }
    }
    
    return COMPLETE;
}
