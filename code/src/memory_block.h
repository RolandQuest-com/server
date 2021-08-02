#pragma once

#include "defines.h"

typedef struct memory_block memory_block;

memory_block* mb_create(u32 chunk_size, u32 chunk_count);
void mb_destroy(memory_block* block);

void* mb_alloc(memory_block* block, u32* handle);
void mb_free(memory_block* block, u32 handle);

u32 mb_chunk_count(memory_block* block);
u32 mb_chunk_size(memory_block* block);
bool mb_in_use(memory_block* block, u32 handle);



