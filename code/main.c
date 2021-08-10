
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "defines.h"
#include "platform.h"
#include "log.h"
#include "packet/request_packet.h"

#include "file.h"

const char* root_path = "C:/Users/Paul/Documents/_Projects/rolandquest.com/html/";
u32 root_path_len = 55;
const char* index_html = "entryTest.html";
u32 index_html_len = 14;

const u32 buf_len = 2048;

void onClientConnect(pcon_handle* client){
    static char addrstr_buffer[256];
    memset(addrstr_buffer, 0, 256);
    platform_address_to_string(client, addrstr_buffer, 256);
    LogTrace("Client connected: %s", addrstr_buffer);
}
void onClientDisconnect(pcon_handle* client){
    
}

typedef struct construct_ret {
    char* ret;
    u32 bytes;
} construct_ret;

construct_ret construct_favicon(char* contents, u32 content_length){
    
    construct_ret ret;
    
    const static char* testResponse = "HTTP/1.1 200 OK\ncontent-type: image/x-icon\nContent-Length: ";
    
    char content_length_str[20];
    sprintf(content_length_str, "%d", content_length);
    
    ret.ret = malloc(strlen(testResponse) + strlen(content_length_str) + 2 + content_length);
    memcpy(ret.ret, testResponse, strlen(testResponse));
    memcpy(ret.ret + strlen(testResponse), content_length_str, strlen(content_length_str));
    ret.ret[strlen(testResponse) + strlen(content_length_str)] = '\n';
    ret.ret[strlen(testResponse) + strlen(content_length_str) + 1] = '\n';
    
    // Gotta be a better way of doing this.
    // strerrorlen_s doesn't work? =/
    memcpy(ret.ret + strlen(testResponse) + strlen(content_length_str) + 2, contents, content_length);
    ret.bytes = strlen(testResponse) + strlen(content_length_str) + 2 + content_length;
    return ret;
}
construct_ret construct(char* contents, u32 content_length){
    
    construct_ret ret;
    
    const static char* testResponse = "HTTP/1.1 200 OK\nContent-Length: ";
    
    char content_length_str[20];
    sprintf(content_length_str, "%d", content_length);
    
    ret.ret = malloc(strlen(testResponse) + strlen(content_length_str) + 2 + content_length);
    memcpy(ret.ret, testResponse, strlen(testResponse));
    memcpy(ret.ret + strlen(testResponse), content_length_str, strlen(content_length_str));
    ret.ret[strlen(testResponse) + strlen(content_length_str)] = '\n';
    ret.ret[strlen(testResponse) + strlen(content_length_str) + 1] = '\n';
    
    // Gotta be a better way of doing this.
    // strerrorlen_s doesn't work? =/
    memcpy(ret.ret + strlen(testResponse) + strlen(content_length_str) + 2, contents, content_length);
    ret.bytes = strlen(testResponse) + strlen(content_length_str) + 2 + content_length;
    return ret;
}

void read_handler(read_info* info){
    
    if(info->eof){
        onClientDisconnect(info->ccon_handle);
        LogInfo(" -> Client closed.");
        platform_close(info->ccon_handle);
        return;
    }
    else if(info->force_closed){
        onClientDisconnect(info->ccon_handle);
        LogInfo(" -> Forced closed.");
        platform_close(info->ccon_handle);
        return;
    }
    
    //NOTE: Need to set packet using {0} so the state is FRESH.
    request_packet p = { 0 };
    request_state s = request_parse(&info->buf, &p);
    
    while(s == INCOMPLETE){
        info->buf.data_pos = 0;
        info->buf.data_len = platform_read(info->ccon_handle, info->buf.buffer, info->buf.buffer_size);
        s = request_parse(&info->buf, &p);
    }
    
    if(s == MALFORMED){
        LogError("MALFORMED");
        LogInfo("Bytes read: %u", info->buf.data_len);
        LogInfo("\n%s", info->buf.buffer);
        return;
    }
    
    //NOTE: Try to read again for another request.
    platform_async_read(info->ccon_handle, 2048, read_handler, 4000);
    
    LogInfo("Bytes read: %u", info->buf.data_len);
    LogInfo("\n%s", info->buf.buffer);
    
    u32 req_path_len = strlen(p.request_path);
    u32 total_path_len = root_path_len;
    total_path_len += (index_html_len > req_path_len) ? index_html_len : req_path_len;
    
    char* full_path;
    full_path = calloc(1, total_path_len);
    memcpy(full_path, root_path, root_path_len);
    
    if(strcmp(p.request_path,"/") == 0){
        memcpy(full_path + root_path_len, index_html, index_html_len);
    }
    else{
        memcpy(full_path + root_path_len, p.request_path, req_path_len);
    }
    
    char* copy = NULL;
    u32 bytes_copied = file_copy_range(&copy, full_path, 0, -1);
    
    construct_ret ret;
    if(strcmp(p.request_path, "/favicon.ico") == 0){
        ret = construct_favicon(copy, bytes_copied);
    }
    else{
        ret = construct(copy, bytes_copied);
    }
    
    platform_send(info->ccon_handle, ret.ret, ret.bytes);
    free(ret.ret);
    free(copy);
}

//TODO:
//TODO:
//TODO: 3x to-do so you know I'm serious.
//TODO: Create a fleshed out buffer struct that all functions take.
//TODO: This will clean up the interface between them.

int main(){
    
    platform_startup();
    log_startup();
    
    pcon_handle* server;
    
    if(!platform_create_server(&server, 8888, 3)){
        LogFatal("Could not initialize server!");
        return 1;
    }
    
    char addrstr_buffer[256];
    platform_address_to_string(server, addrstr_buffer, 256);
    LogInfo("Listening on socket: %s", addrstr_buffer);
    
    bool shutdown_server = false;
    do{
        pcon_handle* client;
        
        LogTrace("Accepting on socket...");
        if(platform_accept(server, &client)){
            
            onClientConnect(client);
            
            //Currently, no clean way to free(client).
            //Handled in my 'platform_async_read' handler for now.
            platform_async_read(client, 2048, read_handler, 4000);
        }
        else{
            LogInfo("Accept failed?");
        }
        
    }while(!shutdown_server);
    
    pcon_destroy(server);
    platform_shutdown();

    return 0;
}


/*

    const u64 runs = 100000000;
    const u64 chunk_size = 2048;
    const u64 chunk_count = 2;
    memory_block* block = mb_create(chunk_size, chunk_count);
    
    stopwatch watch;
    watch_reset(&watch);
    watch_start(&watch);
    for(int i = 0; i < runs; i++){
        //char* test = calloc(chunk_size, 1);
        char* test = malloc(chunk_size);
        test[0] = 't';
        free(test);
    }
    watch_stop(&watch);
    
    LogInfo("Time1: %u", watch_elapsed(&watch));
    
    watch_reset(&watch);
    watch_start(&watch);
    for(int i = 0; i < runs; i++){
        u32 handle;
        char* data = (char*) mb_alloc(block, &handle);
        data[0] = 't';
        mb_free(block, handle);
    }
    watch_stop(&watch);
    
    LogInfo("Time2: %u", watch_elapsed(&watch));
    
    
    watch_reset(&watch);
    watch_start(&watch);
    for(int i = 0; i < runs; i++){
        char test[chunk_size];// = {0};
        test[0] = 't';
    }
    watch_stop(&watch);
    
    LogInfo("Time3: %u", watch_elapsed(&watch));
    
    
    
    mb_destroy(block);
    return 0;
    */