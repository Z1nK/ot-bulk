# ot-bulk

A command-line bulk processor: it reads commands from stdin, groups them into
blocks, and reports each completed block to a console log and a log file.

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

Example input (block size 3, `src/app/bulk/testdata.tsv`):

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
file in the working directory; both outputs are produced asynchronously on
their own worker threads so a slow file write never blocks command intake.

## Building

Requires a C++20 compiler and CMake 3.20+. GoogleTest is fetched
automatically if not found on the system.

```sh
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

## Running

```sh
./build/bin/bulk <block_size> < input.txt
# or interactively / piped:
echo -e "cmd1\ncmd2\ncmd3\ncmd4" | ./build/bin/bulk 3
```

`block_size` defaults to `3` if omitted. A sample input file is available at
`src/app/bulk/testdata.tsv` (copied next to the `bulk` binary on build).

## Testing

```sh
ctest --test-dir build --output-on-failure
```

## Project layout

```
src/
├── app/bulk/            bulk executable (reads stdin, drives the async library)
└── lib/
    ├── async/           command processing, built as a shared/static library
    │   ├── command-parser/  splits an incoming command stream into blocks
    │   ├── executor/        dispatches completed blocks to subscribed observers
    │   ├── datasink/        console / file / async-wrapping output sinks
    │   └── iasync/          public connect()/receive()/disconnect() interface
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
and `Executor` wired to the console and file sinks; `bulk.cpp` never touches
those types directly.
