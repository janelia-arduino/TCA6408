#include <cassert>

#include <TCA6408.h>

namespace {
void dummyCallback() {}
}

int main() {
  static_assert(TCA6408::DEVICE_ADDRESS_7 == 0x27,
                "All TCA6408 addresses should be exposed.");

  {
    TCA6408 tca6408;
    auto read_result = tca6408.readRegisterResult(TCA6408::INPUT_PORT);
    assert(!read_result.ok());
    assert(read_result.error == tca6408::I2cError::NotInitialized);
    assert(!tca6408.communicating());
    assert(tca6408.getLastI2cError() == tca6408::I2cError::NotInitialized);
  }

  {
    TwoWire wire;
    wire.rx_buffer = {0x5a};

    TCA6408 tca6408;
    tca6408.setup(wire, TCA6408::DEVICE_ADDRESS_6);

    assert(!wire.begin_called);
    assert(tca6408.initialized());

    auto read_result = tca6408.readRegisterResult(TCA6408::INPUT_PORT);
    assert(read_result.ok());
    assert(read_result.value == 0x5a);
    assert(wire.transmission_address == TCA6408::DEVICE_ADDRESS_6);
    assert(wire.request_address == TCA6408::DEVICE_ADDRESS_6);
    assert(wire.request_quantity == 1);
    assert(wire.tx_buffer.size() == 1);
    assert(wire.tx_buffer[0] == TCA6408::INPUT_PORT);
    assert(tca6408.getLastI2cError() == tca6408::I2cError::None);

    auto write_result = tca6408.writeRegisterResult(TCA6408::OUTPUT_PORT, 0xa5);
    assert(write_result.ok());
    assert(wire.tx_buffer.size() == 2);
    assert(wire.tx_buffer[0] == TCA6408::OUTPUT_PORT);
    assert(wire.tx_buffer[1] == 0xa5);

    wire.rx_buffer = {0xa5};
    auto output_result = tca6408.readOutputPinResult(TCA6408::PIN_7);
    assert(output_result.ok());
    assert(output_result.value);

    wire.rx_buffer = {0xff};
    auto set_output_result = tca6408.setPinOutputResult(TCA6408::PIN_2);
    assert(set_output_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::CONFIGURATION);
    assert(wire.tx_buffer[1] == 0xfb);

    wire.rx_buffer = {0x00};
    auto set_input_result = tca6408.setPinInputResult(TCA6408::PIN_2);
    assert(set_input_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::CONFIGURATION);
    assert(wire.tx_buffer[1] == 0x04);

    wire.rx_buffer = {0x00};
    auto high_result = tca6408.writeOutputPinResult(TCA6408::PIN_1, true);
    assert(high_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::OUTPUT_PORT);
    assert(wire.tx_buffer[1] == 0x02);

    wire.rx_buffer = {0xff};
    auto low_result = tca6408.writeOutputPinResult(TCA6408::PIN_1, false);
    assert(low_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::OUTPUT_PORT);
    assert(wire.tx_buffer[1] == 0xfd);

    wire.rx_buffer = {0x00};
    auto invert_result = tca6408.setPinPolarityInvertedResult(TCA6408::PIN_3);
    assert(invert_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::POLARITY_INVERSION);
    assert(wire.tx_buffer[1] == 0x08);

    wire.rx_buffer = {0xff};
    auto original_result = tca6408.setPinPolarityOriginalResult(TCA6408::PIN_3);
    assert(original_result.ok());
    assert(wire.tx_buffer[0] == TCA6408::POLARITY_INVERSION);
    assert(wire.tx_buffer[1] == 0xf7);

    wire.rx_buffer = {0x12};
    auto clear_result = tca6408.clearInterruptResult();
    assert(clear_result.ok());
    assert(clear_result.value == 0x12);
  }

  {
    TwoWire wire;
    TCA6408 tca6408;
    tca6408.setup(wire, TCA6408::DEVICE_ADDRESS_0);

    auto invalid_read = tca6408.readInputPinResult(8);
    assert(!invalid_read.ok());
    assert(invalid_read.error == tca6408::I2cError::InvalidPin);

    auto invalid_write = tca6408.writeOutputPinResult(9, true);
    assert(!invalid_write.ok());
    assert(invalid_write.error == tca6408::I2cError::InvalidPin);
  }

  {
    TwoWire wire;
    wire.end_transmission_result = 2;

    TCA6408 tca6408;
    tca6408.setup(wire, TCA6408::DEVICE_ADDRESS_0);

    auto read_result = tca6408.readRegisterResult(TCA6408::CONFIGURATION);
    assert(!read_result.ok());
    assert(read_result.error == tca6408::I2cError::AddressNack);
    assert(tca6408.getLastI2cError() == tca6408::I2cError::AddressNack);
  }

  {
    TwoWire wire;
    wire.request_from_result = 0;
    wire.rx_buffer = {0x11};

    TCA6408 tca6408;
    tca6408.setup(wire, TCA6408::DEVICE_ADDRESS_1);

    auto read_result = tca6408.readRegisterResult(TCA6408::POLARITY_INVERSION);
    assert(!read_result.ok());
    assert(read_result.error == tca6408::I2cError::RequestFailed);
  }

  {
    TwoWire wire;
    wire.request_from_result = 1;

    TCA6408 tca6408;
    tca6408.setup(wire, TCA6408::DEVICE_ADDRESS_2);

    auto read_result = tca6408.readRegisterResult(TCA6408::INPUT_PORT);
    assert(!read_result.ok());
    assert(read_result.error == tca6408::I2cError::ShortRead);
  }

  {
    g_pin_mode_pins.clear();
    g_pin_mode_modes.clear();
    g_digital_write_pins.clear();
    g_digital_write_values.clear();
    g_last_delay_microseconds = 0;

    TCA6408 tca6408;
    tca6408.setResetPin(9);
    tca6408.reset();

    assert(g_pin_mode_pins.size() == 1);
    assert(g_pin_mode_pins[0] == 9);
    assert(g_pin_mode_modes[0] == OUTPUT);
    assert(g_digital_write_pins.size() == 3);
    assert(g_digital_write_pins[0] == 9);
    assert(g_digital_write_values[0] == HIGH);
    assert(g_digital_write_values[1] == LOW);
    assert(g_digital_write_values[2] == HIGH);
    assert(g_last_delay_microseconds == 1);
  }

  {
    g_last_interrupt_pin = 0;
    g_last_interrupt_mode = 0;
    g_last_interrupt_callback = nullptr;

    TCA6408 tca6408;
    tca6408.attachInterrupt(7, dummyCallback);

    assert(g_last_interrupt_pin == 7);
    assert(g_last_interrupt_mode == FALLING);
    assert(g_last_interrupt_callback == dummyCallback);
  }

  return 0;
}
