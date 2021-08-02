#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "platform.h"

bool log_startup(){
    return true;
}

void log_shutdown(){

}

void log_output(log_level level, const char* message, ...){

    const u32 max_msg_length = 4096;
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

    char out_message[max_msg_length];
    memset(out_message, 0, sizeof(out_message));

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, max_msg_length, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[max_msg_length];
    sprintf(out_message2, "[%d]%s%s\n", platform_get_thread_id(), level_strings[level], out_message);

    platform_log_to_stdout(out_message2, level);
}


