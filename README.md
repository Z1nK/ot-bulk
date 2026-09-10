# ot-bulk

A bulk command processor: it groups incoming commands into blocks and reports
each completed block to a console log and a log file. Two front ends feed the
same processing engine:

- **`bulk`** — reads commands from stdin.
- **`bulk-server`** — reads commands from TCP connections, one independent
  block stream per connection.

## Block splitting rules

Commands are grouped into blocks in one of two ways:

- **Static blocks** — commands are grouped in batches of `N` (the block size
  passed on the command line). Once a block reaches `N` commands it is
  considered complete.
- **Dynamic blocks** — a `{` starts a custom block that collects every
  following command until the matching `}` closes it, regardless of size.
  A `{` while a static block is being filled flushes that partial block
  first. Braces may be nested; only the outermost pair delimits a block.
  A dynamic block left open at end of input is discarded, not flushed.

Example input (block size 3, `src/app/bulk-cli/testdata.tsv`):

```
CMD1
CMD2
{
CMD3
CMD4
{
CMD5
}
CMD6
}
CMD7
CMD8
CMD9
CMD10
```

produces the blocks:

```
bulk: CMD1 CMD2
bulk: CMD3 CMD4 CMD5 CMD6
bulk: CMD7 CMD8 CMD9
bulk: CMD10
```

(`CMD1 CMD2` is flushed early because the `{` interrupts the static block;
the nested `{ CMD5 }` doesn't start a new block, it's just part of the outer
one; the trailing `CMD10` is flushed once input ends even though it didn't
reach the block size.)

Each completed block is written to stdout and appended as a `bulkNNN.log`
file in the working directory; both outputs are produced asynchronously so a
slow file write never blocks command intake. The console and file writers
run on a fixed, process-wide pool of worker threads (1 console thread, 2
file threads) shared by every `async::Context` — opening more connections
adds more per-context queues, not more threads.

## Building

Requires a C++20 compiler, CMake 3.20+, and Boost (headers + `Boost::boost`,
used by `bulk-server`'s coroutine-based `TcpServer`). GoogleTest is fetched
automatically if not found on the system.

```sh
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

## Running

### `bulk` (stdin)

```sh
./build/bin/bulk <block_size> < input.txt
# or interactively / piped:
echo -e "cmd1\ncmd2\ncmd3\ncmd4" | ./build/bin/bulk 3
```

`block_size` defaults to `3` if omitted. A sample input file is available at
`src/app/bulk-cli/testdata.tsv` (copied next to the `bulk` binary on build).

### `bulk-server` (TCP)

```sh
./build/bin/bulk-server <port> <block_size>
```

`port` defaults to `9000`, `block_size` to `3` if omitted. Each accepted
connection gets its own block stream (its own `async::Context`), so
concurrent clients don't interleave blocks; a block stream is flushed when
its connection closes. Test it with `nc`:

```sh
printf 'cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n' | nc 127.0.0.1 9000
```

## Testing

```sh
ctest --test-dir build --output-on-failure
```

## Project layout

```
src/
├── app/
│   ├── bulk-cli/            bulk executable (reads stdin, drives the async library)
│   └── bulk-server/         bulk-server executable (reads TCP connections, drives the async library)
└── lib/
    ├── async/           command processing, built as a shared/static library
    │   ├── command-parser/  splits an incoming command stream into blocks
    │   ├── executor/        dispatches completed blocks to subscribed observers
    │   ├── datasink/        console / file sinks, plus the shared worker pool and
    │   │                    per-context mailbox adapter that queue writes onto it
    │   └── iasync/          public connect()/receive()/disconnect() interface
    ├── server/          TcpServer: a minimal Boost.Asio C++20-coroutine TCP server
    ├── concurrency/     thread-safe blocking queue, shared by the datasink layer
    └── time-utils/      timestamp helpers
```

### The `async` library interface

`src/lib/async/iasync/iasync.hpp` is the only header consumers need:

```cpp
async::Context ctx = async::connect(block_size); // start a new command stream
async::receive(ctx, data, size);                 // feed in raw bytes, repeatable
async::disconnect(ctx);                          // flush the trailing block, clean up
```

`Context` is an opaque handle — the caller never interprets it, only passes
it back to `receive()`/`disconnect()`. Internally it owns a `CommandParser`
and `Executor` wired to per-context queues (mailboxes) for the console and
file sinks; those mailboxes are drained by two lazily-created, process-wide
`SharedSinkPool`s (1 thread for console output, 2 for file output) so the
thread count stays fixed no matter how many contexts exist. Both
`bulk.cpp` and `bulk-server`'s per-connection handler only call through this
interface, never touching those types directly.

### The `server` library interface

`src/lib/server/server.hpp` provides `TcpServer`, a small wrapper around
`boost::asio::ip::tcp::acceptor` using C++20 native coroutines
(`boost::asio::awaitable` + `co_spawn`). It accepts connections in a loop and
hands each one to a caller-supplied handler, so the accept/session-spawn
plumbing is reusable across different protocols:

```cpp
boost::asio::io_context io;
TcpServer server(io, {.port = 9000, .on_session = [](tcp::socket socket) {
                        return myHandler(std::move(socket)); // awaitable<void>
                      }});
server.run();
io.run();
```
