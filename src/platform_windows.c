#include "platform.h"

#if defined(PLATFORM_WINDOWS)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#include <stdlib.h>

static HANDLE _mutex = NULL;
static WSADATA _wsa;
static bool _mutexInitialized = false;
static bool _wsaInitialized = false;

struct pcon_handle {
    SOCKET sock;
    struct sockaddr_in svr;
};


bool platform_startup(){
    
    if(!_mutexInitialized){
        _mutex = CreateMutex(NULL, FALSE, NULL);
        _mutexInitialized = (_mutex != NULL);
    }
    if(!_wsaInitialized){
        _wsaInitialized = (WSAStartup(MAKEWORD(2,2),&_wsa) == 0);
    }
    
    return _mutexInitialized && _wsaInitialized;
}
void platform_shutdown(){
    if(_wsaInitialized){
        WSACleanup();
        _wsaInitialized = false;
    }
}


void platform_log_to_stdout(const char* message, u32 color){
    const u8 colors[6] = {
        FOREGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY,
        FOREGROUND_BLUE | BACKGROUND_RED,
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    };

    size_t message_length = strlen(message);
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    platform_lock();
        SetConsoleTextAttribute(console_handle, colors[color]);
        OutputDebugStringA(message);
        WriteConsole(console_handle, message, message_length, NULL, NULL);
    platform_release();
}
void platform_log_to_stderr(const char* message, u32 color){
    const u8 colors[6] = {
        FOREGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY,
        FOREGROUND_BLUE | BACKGROUND_RED,
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
    };

    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    size_t message_length = strlen(message);

    platform_lock();
        SetConsoleTextAttribute(console_handle, colors[color]);
        OutputDebugStringA(message);
        WriteConsole(console_handle, message, message_length, NULL, NULL);
    platform_release();
}


u32 platform_get_thread_id(){
    return GetCurrentThreadId();
}
bool platform_lock(){
    DWORD waitResult = WaitForSingleObject(_mutex, INFINITE);
    switch(waitResult){
        case WAIT_OBJECT_0:
            return true;
        case WAIT_ABANDONED:
            return false;
    }
    return false;
}
bool platform_release(){
    return ReleaseMutex(_mutex);
}


pcon_handle* pcon_create(u16 port){
    pcon_handle* con = calloc(sizeof(pcon_handle), sizeof(char));
    con->svr.sin_family = AF_INET;
    con->svr.sin_addr.s_addr = INADDR_ANY;
    con->svr.sin_port = htons(port);
    return con;
}
void pcon_destroy(pcon_handle* con){
    free(con);
}
bool platform_socket(pcon_handle* con) {
    if((con->sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        return false;
    }
    return true;
}
bool platform_bind(pcon_handle* con) {
    return bind(con->sock, (struct sockaddr*) &con->svr, sizeof(con->svr)) != SOCKET_ERROR;
}
void platform_listen(pcon_handle* con, u32 backlog) {
    listen(con->sock, backlog);
}
bool platform_accept(pcon_handle* scon_handle, pcon_handle** ccon_handle) {
    *ccon_handle = malloc(sizeof(pcon_handle));
    i32 data_size = sizeof(struct sockaddr_in);
    (*ccon_handle)->sock = accept(scon_handle->sock, (struct sockaddr*) &(*ccon_handle)->svr, &data_size);
    return (*ccon_handle)->sock != INVALID_SOCKET;
}
void platform_send(pcon_handle* con, const char* msg, i32 msg_len) {
    send(con->sock, msg, msg_len, 0);
}
u32 platform_read(pcon_handle* con, char* buf, u32 buf_len) {
    return recv(con->sock, buf, buf_len, 0);
}
void platform_close(pcon_handle* con) {
    closesocket(con->sock);
    pcon_destroy(con);
}
void platform_address_to_string(pcon_handle* con, char* buf, i32 buf_len){
    WSAAddressToString((struct sockaddr*) &con->svr, sizeof(con->svr), NULL, buf, (unsigned long*) &buf_len);
}
bool platform_create_server(pcon_handle** svr, u16 port, u32 backlog){
    
    *svr = pcon_create(port);
    
    if(!platform_socket(*svr)){
        pcon_destroy(*svr);
        return false;
    }
    
    if(!platform_bind(*svr)){
        pcon_destroy(*svr);
        return false;
    }
    
    platform_listen(*svr, backlog);
    return true;
}
char* platform_find_ip(const char* hostname){
    
    return NULL;
    
    /*
    static const struct addrinfo hints = {
        0,
        AF_UNSPEC,
        SOCK_STREAM,
        IPPROTO_TCP,
        0,
        NULL,
        NULL,
        NULL
    };
    
    struct addrinfo *result = NULL;
    u32 success = getaddrinfo(hostname, "https", &hints, &result);
     
     if(!success){
         return NULL;
     }
     
    struct sockaddr_in addr_in = *((struct sockaddr_in*) result->ai_addr);
    char* test2 = inet_ntoa(addr_in.sin_addr);
    
    platform_log_to_stdout(test2, 0);
    
    return NULL;
    */
}


typedef struct async_accept_info {
    pcon_handle* scon_handle;
    void (*handler)(accept_info*);
} async_accept_info;
DWORD WINAPI __async_accept(void* info){
    
    async_accept_info* aInfo = (async_accept_info*) info;
    accept_info* ai = malloc(sizeof(accept_info));
    
    ai->error = !platform_accept(aInfo->scon_handle, &(ai->ccon_handle));
    
    aInfo->handler(ai);
    free(aInfo);
    free(ai);
    return 0;
}
bool platform_async_accept(pcon_handle* scon_handle, void (*handler)(accept_info* info)) {
    
    // async_accept_info will be freed in the __async_accept function.
    async_accept_info* aInfo = malloc(sizeof(async_accept_info));
    aInfo->scon_handle = scon_handle;
    aInfo->handler = handler;
    
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(NULL, 0, __async_accept, aInfo, 0, &thread_id);
    
    if (thread_handle == NULL) {
        free(aInfo);
        return false;
    }
    
    CloseHandle(thread_handle);
    return true;
}


typedef struct asyn_read_info {
    pcon_handle* ccon_handle;
    u32 bytes_to_read;
    void (*handler)(read_info*);
} async_read_info;
DWORD WINAPI __async_read(void* info){
    
    async_read_info* arInfo = (async_read_info*) info;
    
    read_info* ri = malloc(sizeof(read_info));
    ri->buffer = calloc(arInfo->bytes_to_read + 1, sizeof(char));
    ri->ccon_handle = arInfo->ccon_handle;
    ri->bytes_read = platform_read(arInfo->ccon_handle, ri->buffer, arInfo->bytes_to_read);
    
    ri->eof = (ri->bytes_read == 0);
    ri->force_closed = (ri->bytes_read == -1);
    
    arInfo->handler(ri);
    free(ri->buffer);
    free(ri);
    free(arInfo);
    return 0;
}
bool platform_async_read(pcon_handle* ccon_handle, u32 bytes_to_read, void (*handler)(read_info* info)) {
    
    // async_read_info will be freed in the __async_read function.
    async_read_info* arInfo = malloc(sizeof(async_read_info));
    arInfo->bytes_to_read = bytes_to_read;
    arInfo->ccon_handle = ccon_handle;
    arInfo->handler = handler;
    
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(NULL, 0, __async_read, arInfo, 0, &thread_id);
    
    if (thread_handle == NULL) {
        free(arInfo);
        return false;
    }
    
    CloseHandle(thread_handle);
    return true;
}




#endif //Windows detection