/*
Notes on the '_s' variants of the file functions.
I have elected to use the '_s' variants when possible.
Some functions have not been implemented by the compiler and will
cause issues. These ones I have not used. E.g. strerrorlen_s

For reference, a passage from https://stackoverflow.com/questions/19396116/how-can-fopen-s-be-more-safe-than-fopen

The s doesn't stand for "safe" in this case, it stands for "security enhanced".
For fopen_s, the parameters are checked for validity before attempting to open the file.

With fopen, you can pass a NULL pointer for the filename and everything will most likely fall to pieces.
fopen_s doesn't have that problem (a).

Keep in mind that these bounds checking interfaces like fopen_s are an optional part of the ISO standard,
detailed in Annex K (as at C11, anyway). Implementations are not required to provide them and, to be honest,
fopen, and many other so-called unsafe functions, are perfectly safe if you know what you're doing as a coder.

It's interesting to note that fopen_s will trap NULL pointers for you but not invalid pointers,
hence why it's security enhanced rather than safe - you can still cause some damage if you pass an invalid but non-NULL pointer.

Other "safe" functions which force you to provide destination buffer sizes are also safe only as long as you pass the right size.
Pass something too big and all bets are off.
*/

/*
Notes on errors.
There are some functions, namely 'fseek' and 'ftell' that can return errors.
I see this as improbable and have elected not to check for this.
Might bite me in the ass later on, but we'll see.
*/

#pragma once

#include <stdio.h>

#include "defines.h"

// Opens a handle to a file.
bool file_open(FILE** file, const char* file_name, const char* mode);

// Closes a file handle.
bool file_close(FILE* file);

// Copies an entire file (up to 4 GB or so) into a buffer.
bool file_copy(char** copy, const char* file_name);

// Copies a range of bytes of a file into a buffer.
bool file_copy_range(char** copy, const char* file_name, u32 byte_start, u32 byte_end);

// Returns the size of a file pointed to by the file handle.
u64 file_size(FILE* file);