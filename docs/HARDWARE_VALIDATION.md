# Hardware Validation

This document records bench validation that was completed before the `2.0.0`
release.

## Tested hardware

- Microcontroller: `pico_w5500_evb`
- Serial port: `/dev/ttyACM0`
- TCA6408 I2C bus: `Wire1`
- `Wire1` SDA pin: `26`
- `Wire1` SCL pin: `27`
- TCA6408 reset pin: `0`
- TCA6408 interrupt pin: `1`
- TCA6408 device address: `TCA6408::DEVICE_ADDRESS_0`

## Tested commands

```sh
pixi run upload examples/PollingRead pico /dev/ttyACM0
pixi run monitor /dev/ttyACM0 115200

pixi run upload examples/InterruptRead pico /dev/ttyACM0
pixi run monitor /dev/ttyACM0 115200
```

## Observed results

### PollingRead

- Repeated reads succeeded with `last_i2c_error: 0`.
- `configuration_register` read back as `0b11111111`, matching input mode.
- `polarity_inversion_register` toggled between `0b0` and `0b11111111`.
- External input toggles changed the non-inverted input register between
  `0b1010` and `0b1000`.
- The corresponding inverted reads changed between `0b11110101` and
  `0b11110111`.

### InterruptRead

- The interrupt example uploaded and ran successfully on the same hardware.
- Input toggles produced interrupt-driven reports with `last_i2c_error: 0`.
- Repeated toggles retriggered correctly, including transitions between
  `0b1010` and `0b1000`.

## Validation status

- Polling input reads: validated on hardware
- Polarity inversion writes/readback: validated on hardware
- Interrupt signaling and deferred input reads: validated on hardware
- Reset pin pulse helper: software-covered only
- Output pin drive behavior: not bench-validated in this setup because the
  connected TCA6408 wiring was input-only
