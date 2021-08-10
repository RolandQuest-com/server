#pragma once

#include "defines.h"
#include "char_buffer.h"

//TODO: Potentially break this file up.
//TODO: Seems to be a lot of functionality in a single header.

// Platform detection
#if defined(_WIN64)
#define PLATFORM_WINDOWS
#define PLATFORM_WINDOWS_64
#define PLATFORM_ALIGNMENT 8
#elif defined(_WIN32)
#define PLATFORM_WINDOWS
#define PLATFORM_WINDOWS_32
#define PLATFORM_ALIGNMENT 4
#elif defined(__linux__)
// Note that multiple platforms use linux.
// They all use this define, but to distinguish you'd have to use other predefined macros.
#define PLATFORM_LINUX
#endif

#ifndef PLATFORM_ALIGNMENT
#error "PLATFORM_ALIGNMENT needs to be defined."
#endif


// Unexposed handle for a socket.
typedef struct pcon_handle pcon_handle;

// Initiates the system to work with sockets.
// Call before any other functions.
bool platform_startup();
// Closes the system to work with sockets.
void platform_shutdown();

// Logs a message to stdout. Console by default.
void platform_log_to_stdout(const char* message, u32 color);
// Logs a message to stderr. Console by default.
void platform_log_to_stderr(const char* message, u32 color);

// Returns the id of the calling thread.
u32 platform_get_thread_id();
// Locks the platform mutex.
bool platform_lock();
// Releases the platform mutex.
bool platform_release();

// Creates a skeleton socket handle.
pcon_handle* pcon_create(u16 port);
// Destroys a socket handle.
void pcon_destroy(pcon_handle* con);
// Creates a socket and assigns to given handle.
bool platform_socket(pcon_handle* con);
// Binds socket of handle.
bool platform_bind(pcon_handle* con);
// Begins listening of given socket handle.
void platform_listen(pcon_handle* con, u32 backlog);
// Accepts on socket handle 'scon_handle'.
// New socket is returned through 'ccon_handle'.
bool platform_accept(pcon_handle* scon_handle, pcon_handle** ccon_handle);
// Sends data through given socket.
void platform_send(pcon_handle* con, const char* msg, i32 msg_len);
// Determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
int platform_readable(pcon_handle* con, u32 timeout);
// Reads data from given socket.
u32 platform_read(pcon_handle* con, char* buf, u32 buf_len);
// Closes socket and calls 'free' on handle.
void platform_close(pcon_handle* con);
// Fills 'buf' with human readable ip address of 'con'.
void platform_address_to_string(pcon_handle* con, char* buf, i32 buf_len);
// Creates a server socket handle ready to accept.
// Calls 'platform_socket', 'platform_bind', then 'platform_listen'.
bool platform_create_server(pcon_handle** svr, u16 port, u32 backlog);


// TODO: Clean this up.
char* platform_find_ip(const char* hostname);





// Info passed to the handler of a 'platform_async_accept' handler.
typedef struct accept_info {
    pcon_handle* ccon_handle;
    bool error;
} accept_info;

// Calls 'accept' on a newly created thread.
// 'handler' is called when 'accept' returns.
// 'handler' assumes ownership of 'ccon_handle' of the accept_info struct.
// TODO: Pass error info to handler.
bool platform_async_accept(pcon_handle* scon_handle, void (*handler)(accept_info* info));

// Info passed to the handler of a 'platform_async_read' handler.
typedef struct read_info {
    pcon_handle* ccon_handle;
    char_buffer buf;
    bool eof;
    bool force_closed;
} read_info;

// Calls 'read' on a newly created thread.
// 'handler' is called when 'read' returns.
// 'handler' assumes ownership of 'ccon_handle' of the accept_info struct.
// TOOD: Pass error info to handler.
bool platform_async_read(pcon_handle* ccon_handle, u32 bytes_to_read, void (*handler)(read_info* info), u32 timeout);


