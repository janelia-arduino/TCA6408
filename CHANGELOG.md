# Changelog

## 2.0.0

This release hardens the library for repeatable bring-up, release validation,
and clearer application-level use.

Highlights:

- explicit caller-owned `TwoWire` setup instead of hidden `Wire.begin()`
- typed I2C results via `tca6408::Result<T>` and `tca6408::I2cError`
- full `0x20` through `0x27` device-address coverage
- safer register transactions with explicit request / short-read handling
- per-pin helpers for input reads, direction control, polarity control, output
  latch readback, and output writes
- explicit `clearInterrupt()` helper matching the datasheet interrupt-clear
  behavior
- optional reset pulse helper for the active-low `RESET` pin
- native unit tests for register semantics, pin helpers, invalid-pin handling,
  reset, and interrupt attachment
- Pixi-based local workflow for install, build, upload, monitor, version sync,
  and release checks
- hardware-validated polling and interrupt examples on RP2040 Pico hardware

Known validation scope at release time should be read from
`docs/HARDWARE_VALIDATION.md`.
