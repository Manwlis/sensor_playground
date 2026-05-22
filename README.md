# sensor_playground
Just experimenting with some sensors and the Nucleo-STM32H755ZI

* Read internal temp sensor using DMA, which is fired every one second by a hardware timer.
* Control LIS3DHTR using I2C in blocking & non-blocking modes.

## ADC Driver

## LIS3DHTR Driver

## Lightweight Logging (LWL)
LWL is a lightweight logging system designed for low runtime and memory overhead. It is intended primarily for crash and failure analysis, by enabling high-granularity logging during normal operation with minimal impact on system performance. It is most useful when combined with a more verbose, higher-level logging system.

### Firmware Implementation
LWL consists of a circular buffer stored in memory and a minimal API for writing and extracting log data. It supports variable-length records containing primitive data types.

#### API:
- `lwl_init()`: Initializes or resets the module by clearing the buffer.
- `lwl_enter_record()`: Main logging function. Appends a record to the buffer. See the following section for format requirements and limitations.
- `dump_log()`: Exports the buffer contents and next write position through UART in binary format for post-mortem decoding.

The implementation tracks only the next write position in the buffer. Records cannot be inspected during runtime; the buffer is intended to be extracted and decoded post-mortem.

### lwl_enter_record() Format
lwl_enter_record() is a variadic function. To ensure that it functions correctly and it is compatible with the decoding script that parses the codebase, all log writes must use the following format:

```c
lwl_enter_record( MODULE_ID, FUNCTION_ID, "argument_types", arguments... );
```

Example:
```c
lwl_enter_record(
    LIS3DHTR_LWL_ID,
    LIS3DHTR_READ_LWL_ID,
    "cc",
    device->i2c_address,
    device->memory_map[reg_address].address
);
```

#### IDs
`MODULE_ID` and `FUNCTION_ID` must be macros defined in `lwl.h` and must be single byte values. Preferably single character literals to improve readability of the logs.

Example:
```c
#define LIS3DHTR_LWL_ID      'L'
#define LIS3DHTR_READ_LWL_ID 'R'
```

The `( MODULE_ID, FUNCTION_ID )` combination must be globally unique across the project.

#### Argument types
The third argument is a format string describing the types of the remaining arguments.

Supported types:
| Type | Size | Meaning |
|------|------|------|
| `c` | 1 byte | uint8_t / byte |
| `s` | 2 bytes | int16_t |
| `h` | 2 bytes | uint16_t |
| `d` | 4 bytes | int32_t |
| `u` | 4 bytes | uint32_t |
| `f` | 4 bytes | float |

The number and order of format characters must match the provided arguments.

Examples:
```c
lwl_enter_record( MOD, FUNC, "" );
lwl_enter_record( MOD, FUNC, "c", status );
lwl_enter_record( MOD, FUNC, "uh", timestamp, value );
lwl_enter_record( MOD, FUNC, "f", temperature );
```

### Decoding script

The script `lwl_decoder.py` decodes the binary dumps of the LWL circular buffers.

The script parses the firmware source tree and automatically builds a database of all `lwl_enter_record()` calls, including:
- module IDs and function IDs
- argument names
- format strings
- entry sizes

The decoder then reads a binary buffer dump (`dump.bin`) and reconstructs the logged records using the generated database.

Requirements:
- The source tree used by the script **must match the firmware version** that generated the dump. Changes to log definitions, IDs, formats or sizes can make decoding invalid.
- The dump directory must contain an `info.txt` file with a line:

  ```text
  next_entry_index = <number>
  ```

  where `<number>` is the buffer write index.

The script automatically:
- resolves ID macros from `lwl.h`
- unwraps the circular buffer
- detects the non-wrapped buffer case
- determines the most likely decoding start point
- decodes and prints the recovered log entries in tabular form.
