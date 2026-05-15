# Traffic Light Controller

A simple traffic light controller modeled in SystemC.

## Overview

This project simulates a traffic light controller using a finite state machine implemented as a SystemC module. The controller cycles through RED, GREEN, and YELLOW states with configurable durations.

## Module Specification

### `TrafficLight` (SC_MODULE)

| Element | Type | Description |
|---|---|---|
| `clk` | `sc_in<bool>` | Clock input port |
| `RED` | `sc_out<bool>` | Red light output |
| `YELLOW` | `sc_out<bool>` | Yellow light output |
| `GREEN` | `sc_out<bool>` | Green light output |

### State Machine

The controller uses an `SC_THREAD` process to cycle through states in the following order:

```
RED (3 cycles) → GREEN (3 cycles) → YELLOW (1 cycle) → repeat
```

State transitions are printed to the console with `sc_time_stamp()` at each change.

## Testbench

`sc_main` drives the module for **30 clock cycles** to exercise the full state sequence.

## Prerequisites

- SystemC 2.3.x or later
- A C++ compiler with C++11 support (GCC, MSVC, Clang)

## Building

```bash
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -o traffic_light main.cpp
```

## Running

```bash
./traffic_light
```

Expected output shows state transitions at each clock edge with simulation timestamps.
