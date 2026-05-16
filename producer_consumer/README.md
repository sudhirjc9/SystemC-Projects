# Producer-Consumer (SystemC FIFO)

A SystemC simulation demonstrating producer-consumer communication over a bounded FIFO channel.

## Overview

Two modules — `Producer` and `Consumer` — communicate through an `sc_fifo<int>` of depth 4. The producer writes integers 1–10 one per clock cycle; the consumer reads each value and introduces a 2-cycle processing delay before reading the next.

## Module Specification

### `Producer`

| Element | Type | Description |
|---|---|---|
| `clk` | `sc_in<bool>` | Clock input |
| `out` | `sc_fifo_out<int>` | FIFO write port |

- Writes integers **1 to 10**, one value per clock cycle.
- Prints `[time] Producer wrote: X` after each write.

### `Consumer`

| Element | Type | Description |
|---|---|---|
| `clk` | `sc_in<bool>` | Clock input |
| `in` | `sc_fifo_in<int>` | FIFO read port |

- Reads one integer from the FIFO per read operation.
- Prints `[time] Consumer read: X` after each read.
- Waits **2 clock cycles** after each read before reading the next (processing delay).

## State / Timing Diagram

```
Cycle:    1    2    3    4    5    6    7    8    9   10
Producer: W1   W2   W3   W4   W5   W6   W7   W8   W9  W10
Consumer: R1   --   --   R2   --   --   R3   --   --  ...

FIFO (depth 4) absorbs bursts while the consumer catches up.
```

## Testbench (`sc_main`)

| Component | Details |
|---|---|
| `sc_fifo<int>` | Depth 4, connects Producer → Consumer |
| Clock | 1 ns period |
| Simulation | Runs until all 10 values are consumed |

## Prerequisites

- SystemC 2.3.x or later
- C++ compiler with C++11 support (GCC, MSVC, Clang)

## Building

```bash
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -std=c++11 \
    -o producer_consumer producer_consumer.cpp
```

## Running

```bash
./producer_consumer
```

### Expected Output (excerpt)

```
0 ns | Producer wrote: 1
0 ns | Consumer read:  1
1 ns | Producer wrote: 2
3 ns | Consumer read:  2
2 ns | Producer wrote: 3
...
```

The consumer reads lag behind the producer because of the 2-cycle processing delay; the FIFO buffers up to 4 unread values before the producer blocks.
