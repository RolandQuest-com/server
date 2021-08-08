#include "request_packet.h"

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
request_state __extract_method_line(char* data, u32 size, request_packet* packet, u32* i){
    
    // METHOD - PATH - VERSION
    char con[3][MAX_REQUEST_PATH_LENGTH] = {{0}};
    //TODO: Need to make sure you don't read over the array size.
    //TODO: Need to make sure not reading over parameter 'size'.
    
    u32 data_pos = *i;
    u32 arr_index = 0;
    u32 arr_pos = 0;
    
    while(arr_index < 3){
        
        char c = data[data_pos];
        data_pos++;
        con[arr_index][arr_pos] = c;
        arr_pos++;
        
        if(c == ' ' || c == '\r'){
            con[arr_index][arr_pos - 1] = '\0';
            arr_index++;
            arr_pos = 0;
        }
    }
    
    // TODO: Assuming next char is '\n' and not checking.
    data_pos++;
    
    const char* method = __find_method(con[0]);
    const char* version = __find_version(con[2]);
    
    if(method == NULL || version == NULL){
        return MALFORMED;
    }
    
    packet->http_method = method;
    packet->http_version = version;
    memcpy(packet->request_path, con[1], MAX_REQUEST_PATH_LENGTH);
    *i = data_pos;
    return COMPLETE;
}
request_state __extract_headers(char* data, u32 size, request_packet* packet, u32* i){
    
    //TODO: Need to make sure you don't read over the array size.
    //TODO: Need to make sure not reading over parameter size.
    
    u32 data_pos = *i;
    bool complete = false;
    u32 header_index = 0;
    
    while(!complete){
        
        char con[2][MAX_HEADER_LENGTH] = {0};
        bool delimiter_found = false;
        u32 arr_index = 0;
        u32 arr_pos = 0;
        
        while(arr_index < 2){
            
            char c = data[data_pos];
            data_pos++;
            con[arr_index][arr_pos] = c;
            arr_pos++;
            
            // TODO: Assuming first ':' is the delimiter. Check specification.
            if((c == ':' && !delimiter_found) || c == '\r'){
                delimiter_found = true;
                con[arr_index][arr_pos - 1] = '\0';
                data_pos++;
                arr_index++;
                arr_pos = 0;
            }
        }
        
        memcpy(packet->headers.headers[header_index].head, con[0], MAX_HEADER_LENGTH);
        memcpy(packet->headers.headers[header_index].body, con[1], MAX_HEADER_LENGTH);
        header_index++;
        
        // TODO: Assuming next char is '\n' and not checking.
        if(data[data_pos] == '\r'){
            data_pos += 2;
            complete = true;
        }
        if(header_index == MAX_HEADER_COUNT){
            complete = true;
        }
        
    }
    
    *i = data_pos;
    return COMPLETE;
}


//NOTE: Will return MALFORMED if all headers are not provided.
//NOTE: Content data can be done piecemeal.
//TODO: Read content data.
//TODO: Handle chunked content data.
request_state request_parse(char* data, u32 size, request_packet* packet){
    
    if(packet->pstate == MALFORMED || packet->pstate == COMPLETE){
        return packet->pstate;
    }
    
    u32 i = 0;
    
    if(packet->pstate == FRESH){
        
        if(__extract_method_line(data, size, packet, &i) == MALFORMED){
            return MALFORMED;
        }
        if(__extract_headers(data, size, packet, &i) == MALFORMED){
            return MALFORMED;
        }
    }
    
    //__extract_content();
    return COMPLETE;
}
