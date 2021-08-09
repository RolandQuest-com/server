#include "file.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "log.h"

bool file_open(FILE** file, const char* file_name, const char* mode){
    errno_t e = fopen_s(file, file_name, mode);
    if(*file == NULL){
        
        // Compiler does not like 'strerrorlen_s' here.
        size_t e_len = 256;
        char e_buf[e_len];
        
        LogError("Error opening file: %s", strerror_s(e_buf, e_len, e));
        return false;
    }
    return true;
}
bool file_close(FILE* file){
    return fclose(file) == 0;
}
bool file_copy(char** copy, const char* file_name){
    return file_copy_range(copy, file_name, 0, -1);
}
u64 file_copy_range(char** copy, const char* file_name, u32 byte_start, u32 byte_end){
    
    // Lots of places this can fail with no error catching.
    // TODO: Implement error handling.
    
    if(byte_end < byte_start){
        return -1;
    }
    
    FILE* file;
    if(!file_open(&file, file_name, "r")){
        return -1;
    }
    
    u64 file_length = file_size(file);
    
    if(byte_start > file_length - 1){
        fclose(file);
        return -1;
    }
    if(byte_end > file_length - 1){
        byte_end = file_length - 1;
    }
    
    u32 bytes_to_read = byte_end - byte_start + 1;
    *copy = malloc(sizeof(char) * (bytes_to_read + 1));
    
    fseek(file, byte_start, SEEK_SET);
    size_t char_read = fread(*copy, sizeof(char), bytes_to_read, file);
    (*copy)[char_read] = '\0';
    fclose(file);
    return char_read;
}
u64 file_size(FILE* file){
    u64 current_pos = ftell(file);
    fseek(file, 0L, SEEK_END);
    u64 size = ftell(file);
    fseek(file, current_pos, SEEK_SET);
    return size;
}

