# Hierarchical Module — Memory Subsystem

A SystemC design demonstrating module hierarchy: a parent `MemorySubsystem` that internally instantiates `RAM` and `ROM` submodules connected via internal signal wires.

## System Architecture

```
         ┌─────────────────────────┐
         │      MemorySubsystem    │
         │                         │
 addr ──►│──► AddressDecoder       │
 data ──►│       │                 │
 we   ──►│    ┌──┴───┐  ┌────────┐│
         │    │ RAM  │  │  ROM   ││
         │    └──────┘  └────────┘│
         │       │          │      │
 out  ◄──│───────┴──────────┘      │
         └─────────────────────────┘
```

## Module Specification

### `RAM`

| Port | Type | Direction | Description |
|---|---|---|---|
| `addr` | `sc_uint<8>` | in | 8-bit address (0–255) |
| `data_in` | `sc_uint<8>` | in | Data to write |
| `we` | `bool` | in | Write-enable (active high) |
| `data_out` | `sc_uint<8>` | out | Data read output |

- Storage: `sc_uint<8> mem[256]`
- Process: `SC_METHOD`, sensitive to `we`
- On `we` high: writes `data_in` to `mem[addr]` and drives `data_out`

### `ROM`

| Port | Type | Direction | Description |
|---|---|---|---|
| `addr` | `sc_uint<8>` | in | 8-bit address |
| `data_out` | `sc_uint<8>` | out | Pre-loaded data output |

- Pre-loaded: `mem[i] = i * 2` for all `i` in 0–255
- Process: `SC_METHOD`, sensitive to `addr`
- Combinationally drives `data_out = mem[addr]`

### `MemorySubsystem` (parent)

| Port | Type | Direction | Description |
|---|---|---|---|
| `addr` | `sc_uint<8>` | in | RAM address |
| `data_in` | `sc_uint<8>` | in | RAM write data |
| `we` | `bool` | in | RAM write-enable |
| `ram_out` | `sc_uint<8>` | out | RAM read output |
| `rom_addr` | `sc_uint<8>` | in | ROM address |
| `rom_out` | `sc_uint<8>` | out | ROM read output |

- Instantiates `RAM` and `ROM` as submodules via constructor initializer list
- Connects submodules using internal `sc_signal` wires

## Testbench (`sc_main`)

| Operation | Address | Data |
|---|---|---|
| RAM write | `0x00` | `0xAA` |
| RAM write | `0x01` | `0xBB` |
| RAM write | `0x02` | `0xCC` |
| ROM read | `5` | expected `10` (`5 × 2`) |
| ROM read | `10` | expected `20` (`10 × 2`) |
| ROM read | `20` | expected `40` (`20 × 2`) |

All results are printed with `sc_time_stamp()`.

## Prerequisites

- SystemC 2.3.x or later
- C++ compiler with C++11 support (GCC, MSVC, Clang)

## Building

```bash
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -std=c++11 \
    -o memory_subsystem memory_subsystem.cpp
```

## Running

```bash
./memory_subsystem
```

### Expected Output

```
0 s | RAM[0x00] <= 0xAA
0 s | RAM[0x01] <= 0xBB
0 s | RAM[0x02] <= 0xCC
0 s | ROM[5]  => 10
0 s | ROM[10] => 20
0 s | ROM[20] => 40
```

## Simulation Output

![Simulation output](!(image.png))
