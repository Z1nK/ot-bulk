#pragma once
#include <cstddef>

namespace async {

using Context = void*;


// Creates a new command-processing context with the given static command
// block size. The returned value is opaque to the caller and must be passed
// unchanged to receive() and disconnect().
Context connect(std::size_t bulk_size);

// Feeds raw bytes into the context. Repeatable: the caller may invoke this
// any number of times to stream multiple commands in succession. Buffer
// boundaries need not align with command boundaries.
void receive(Context context, const char* data, std::size_t size);

// Flushes any pending partial command block and destroys the context. After
// this call the context must not be used again.
void disconnect(Context context);

}  // namespace async
