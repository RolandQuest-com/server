#include "memory_block.h"

#include <stdlib.h>
#include <string.h>

#include "platform.h"

/*
Memory Layout
 ------------------------------------------------------------------------
| memory_block struct | in_use array | chunk 1 | chunk 2 | ... | chunk N |
 ------------------------------------------------------------------------
 
 Implementation Details
 ----------------------
 memory_block::next_free
 -1 : No free chunk.
 Otherwise : Index of a free chunk.
 
*/

struct memory_block {
    u32 chunk_size;
    u32 chunk_count;
    bool* in_use;
    void* first_chunk;
    u32 next_free;
};

memory_block* mb_create(u32 chunk_size, u32 chunk_count) {
    
    //TODO: Check that memory is within limits.
    //TODO: Align the memory. (u64 bytes_needed = bytes + PLATFORM_ALIGNMENT - (bytes % PLATFORM_ALIGNMENT);)
    
    memory_block* block = malloc(sizeof(memory_block) + sizeof(bool) * chunk_count + chunk_size * chunk_count);
    memset(block, 0, sizeof(memory_block) + sizeof(bool) * chunk_count);
    block->chunk_size = chunk_size;
    block->chunk_count = chunk_count;
    block->in_use = (bool*) (block + 1);
    block->first_chunk = (void*) (block->in_use + chunk_count);
    block->next_free = 0;
    return block;
}
void mb_destroy(memory_block* block) {
    free(block);
}

void* mb_alloc(memory_block* block, u32* handle) {
    
    if(block->next_free == -1){
        return NULL;
    }
    
    *handle = block->next_free;
    block->in_use[block->next_free] = true;
    void* to_ret = block->first_chunk + block->chunk_size * block->next_free;
    //memset(to_ret, 0, block->chunk_size);
    
    u32 curpos = block->next_free;
    do {
        block->next_free = ++block->next_free % block->chunk_count;
        if(!block->in_use[block->next_free]){
            return to_ret;
        }
    }while(curpos != block->next_free);
    
    block->next_free = -1;
    return to_ret;
}
void mb_free(memory_block* block, u32 handle) {
    block->in_use[handle] = false;
}

u32 mb_chunk_count(memory_block* block){
    return block->chunk_count;
}
u32 mb_chunk_size(memory_block* block){
    return block->chunk_size;
}
bool mb_in_use(memory_block* block, u32 handle){
    return block->in_use[handle];
}


