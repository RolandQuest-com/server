
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

const u32 buf_len = 2048;

void onClientConnect(pcon_handle* client){
    static char addrstr_buffer[256];
    memset(addrstr_buffer, 0, 256);
    platform_address_to_string(client, addrstr_buffer, 256);
    LogTrace("Client connected: %s", addrstr_buffer);
}
void onClientDisconnect(pcon_handle* client){
    
}

char* construct(char* contents){
    
    const static char* testResponse = "HTTP/1.1 200 OK\nContent-Length: ";
    
    u32 content_length = strlen(contents);
    char content_length_str[20];
    sprintf(content_length_str, "%d", content_length);
    
    char* toSend = malloc(strlen(testResponse) + strlen(content_length_str) + 2 + content_length);
    memcpy(toSend, testResponse, strlen(testResponse));
    memcpy(toSend + strlen(testResponse), content_length_str, strlen(content_length_str));
    toSend[strlen(testResponse) + strlen(content_length_str)] = '\n';
    toSend[strlen(testResponse) + strlen(content_length_str) + 1] = '\n';
    
    // Gotta be a better way of doing this.
    // strerrorlen_s doesn't work? =/
    memcpy(toSend + strlen(testResponse) + strlen(content_length_str) + 2, contents, content_length);
    
    return toSend;
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
    request_state s = request_parse(info->buffer, info->bytes_read, &p);
    
    while(s == INCOMPLETE){
        platform_read(info->ccon_handle, info->buffer, info->buffer_size);
        s = request_parse(info->buffer, info->bytes_read, &p);
    }
    
    if(s == MALFORMED){
        // Send error.
    }
    if(s == COMPLETE){
        // Handle packet.
        return;
    }
    if(s == INCOMPLETE){
        
    }
    
    // COMPLETE OR NEED_MORE_CONTENT
    
    /*
    while(s.pstate != COMPLETE){
        // WARNING: The byte after bytes_read is not guaranteed to be '\0'.
        u32 bytes_read = platform_read(info->ccon_handle, info->buffer, info->buffer_size);
        request_parse(info->buffer, bytes_read, &p, &s);
    }
    */
    
    LogInfo("Bytes read: %u", info->bytes_read);
    LogInfo("\n%s", info->buffer);
    
    if(info->bytes_read < buf_len - 1){
        char* copy = NULL;
        file_copy_range(&copy, "C:\\Users\\Paul\\Documents\\_Projects\\rolandquest.com\\html\\entryTest.html", 0, -1);
        
        char* toSend = construct(copy);
        platform_send(info->ccon_handle, toSend, strlen(toSend));
        free(toSend);
        free(copy);
    }
    
}
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