// ----------------------------------------------------------------------------
// TCA6408.h
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------

#ifndef TCA6408_H
#define TCA6408_H
#include <Arduino.h>
#include <Wire.h>

namespace tca6408 {
enum class I2cError : uint8_t {
  None = 0,
  NotInitialized,
  InvalidPin,
  DataTooLong,
  AddressNack,
  DataNack,
  OtherBusError,
  RequestFailed,
  ShortRead,
};

template <typename T> struct Result {
  T value{};
  I2cError error{I2cError::None};

  constexpr bool ok() const {
    return error == I2cError::None;
  }
};

template <> struct Result<void> {
  I2cError error{I2cError::None};

  constexpr bool ok() const {
    return error == I2cError::None;
  }
};
}

class TCA6408 {
public:
  TCA6408();
  static const uint8_t PIN_COUNT = 8;

  enum DeviceAddress {
    DEVICE_ADDRESS_0 = 0x20,
    DEVICE_ADDRESS_1 = 0x21,
    DEVICE_ADDRESS_2 = 0x22,
    DEVICE_ADDRESS_3 = 0x23,
    DEVICE_ADDRESS_4 = 0x24,
    DEVICE_ADDRESS_5 = 0x25,
    DEVICE_ADDRESS_6 = 0x26,
    DEVICE_ADDRESS_7 = 0x27,
  };

  // The caller owns I2C peripheral configuration and should call wire.begin()
  // before setup().
  void setup(TwoWire& wire, DeviceAddress device_address);
  bool initialized() const {
    return wire_ptr_ != nullptr;
  }

  enum RegisterAddress {
    INPUT_PORT = 0,
    OUTPUT_PORT = 1,
    POLARITY_INVERSION = 2,
    CONFIGURATION = 3
  };
  enum Pin : uint8_t {
    PIN_0 = 0,
    PIN_1 = 1,
    PIN_2 = 2,
    PIN_3 = 3,
    PIN_4 = 4,
    PIN_5 = 5,
    PIN_6 = 6,
    PIN_7 = 7,
  };
  tca6408::Result<uint8_t>
  readRegisterResult(RegisterAddress register_address) {
    tca6408::Result<uint8_t> result;

    if (wire_ptr_ == nullptr) {
      result.error = tca6408::I2cError::NotInitialized;
      last_i2c_error_ = result.error;
      return result;
    }

    wire_ptr_->beginTransmission(device_address_);
    wire_ptr_->write((uint8_t)register_address);
    result.error = endTransmissionToError_(wire_ptr_->endTransmission());
    if (!result.ok()) {
      last_i2c_error_ = result.error;
      return result;
    }

#if defined(WIRE_HAS_TIMEOUT)
    wire_ptr_->clearWireTimeoutFlag();
#endif
    uint8_t bytes_requested = 1;
    uint8_t bytes_received =
      wire_ptr_->requestFrom((uint8_t)device_address_, bytes_requested);
    if (bytes_received != bytes_requested) {
      result.error = tca6408::I2cError::RequestFailed;
      last_i2c_error_ = result.error;
      return result;
    }

    if (wire_ptr_->available() < bytes_requested) {
      result.error = tca6408::I2cError::ShortRead;
      last_i2c_error_ = result.error;
      return result;
    }

    result.value = (uint8_t)wire_ptr_->read();
    last_i2c_error_ = result.error;
    return result;
  }
  tca6408::Result<void> writeRegisterResult(RegisterAddress register_address,
                                            uint8_t data) {
    tca6408::Result<void> result;

    if (wire_ptr_ == nullptr) {
      result.error = tca6408::I2cError::NotInitialized;
      last_i2c_error_ = result.error;
      return result;
    }

    wire_ptr_->beginTransmission(device_address_);
    wire_ptr_->write((uint8_t)register_address);
    wire_ptr_->write(data);
    result.error = endTransmissionToError_(wire_ptr_->endTransmission());
    last_i2c_error_ = result.error;
    return result;
  }
  uint8_t readRegister(RegisterAddress register_address);
  void writeRegister(RegisterAddress register_address, uint8_t data);
  tca6408::I2cError getLastI2cError() const {
    return last_i2c_error_;
  }
  bool communicating() {
    return readRegisterResult(INPUT_PORT).ok();
  }

  uint8_t readInputRegister();
  tca6408::Result<bool> readInputPinResult(uint8_t pin) {
    return readRegisterBitResult(INPUT_PORT, pin);
  }
  bool readInputPin(uint8_t pin) {
    return readInputPinResult(pin).value;
  }
  tca6408::Result<uint8_t> clearInterruptResult() {
    return readRegisterResult(INPUT_PORT);
  }
  uint8_t clearInterrupt() {
    return clearInterruptResult().value;
  }

  // If a bit in the polarity inversion register is set to 1 the corresponding
  // port pin is inverted polarity
  uint8_t readPolarityInversionRegister();
  void writePolarityInversionRegister(uint8_t data);
  tca6408::Result<bool> readPinPolarityInvertedResult(uint8_t pin) {
    return readRegisterBitResult(POLARITY_INVERSION, pin);
  }
  bool pinPolarityInverted(uint8_t pin) {
    return readPinPolarityInvertedResult(pin).value;
  }
  tca6408::Result<void> setPinPolarityInvertedResult(uint8_t pin) {
    return updateRegisterBitResult(POLARITY_INVERSION, pin, true);
  }
  void setPinPolarityInverted(uint8_t pin) {
    setPinPolarityInvertedResult(pin);
  }
  tca6408::Result<void> setPinPolarityOriginalResult(uint8_t pin) {
    return updateRegisterBitResult(POLARITY_INVERSION, pin, false);
  }
  void setPinPolarityOriginal(uint8_t pin) {
    setPinPolarityOriginalResult(pin);
  }
  void setAllPinsPolarityOriginal();
  void setAllPinsPolarityInverted();

  // If a bit in the configuration register is set to 1 the corresponding port
  // pin is enabled as an input
  uint8_t readConfigurationRegister();
  void writeConfigurationRegister(uint8_t data);
  tca6408::Result<bool> readPinInputModeResult(uint8_t pin) {
    return readRegisterBitResult(CONFIGURATION, pin);
  }
  bool pinIsInput(uint8_t pin) {
    return readPinInputModeResult(pin).value;
  }
  tca6408::Result<void> setPinInputResult(uint8_t pin) {
    return updateRegisterBitResult(CONFIGURATION, pin, true);
  }
  void setPinInput(uint8_t pin) {
    setPinInputResult(pin);
  }
  tca6408::Result<void> setPinOutputResult(uint8_t pin) {
    return updateRegisterBitResult(CONFIGURATION, pin, false);
  }
  void setPinOutput(uint8_t pin) {
    setPinOutputResult(pin);
  }
  void setAllPinsInput();
  void setAllPinsOutput();

  uint8_t readOutputRegister();
  tca6408::Result<bool> readOutputPinResult(uint8_t pin) {
    return readRegisterBitResult(OUTPUT_PORT, pin);
  }
  bool readOutputPin(uint8_t pin) {
    return readOutputPinResult(pin).value;
  }
  void writeOutputRegister(uint8_t data);
  tca6408::Result<void> writeOutputPinResult(uint8_t pin, bool value) {
    return updateRegisterBitResult(OUTPUT_PORT, pin, value);
  }
  void writeOutputPin(uint8_t pin, bool value) {
    writeOutputPinResult(pin, value);
  }
  void setPinHigh(uint8_t pin) {
    writeOutputPin(pin, true);
  }
  void setPinLow(uint8_t pin) {
    writeOutputPin(pin, false);
  }

  void setResetPin(uint8_t reset_pin);
  void reset();
  typedef void (*voidFuncPtr)(void);
  void attachInterrupt(uint8_t interrupt_pin, voidFuncPtr callback);

private:
  TwoWire* wire_ptr_;
  DeviceAddress device_address_;
  uint8_t reset_pin_;
  tca6408::I2cError last_i2c_error_;
  const static uint8_t NO_PIN = 255;
  bool pinValid_(uint8_t pin) const {
    return pin < PIN_COUNT;
  }
  uint8_t bitMaskForPin_(uint8_t pin) const {
    return (uint8_t)(1u << pin);
  }
  tca6408::Result<bool> readRegisterBitResult(RegisterAddress register_address,
                                              uint8_t pin) {
    tca6408::Result<bool> result;
    if (!pinValid_(pin)) {
      result.error = tca6408::I2cError::InvalidPin;
      last_i2c_error_ = result.error;
      return result;
    }

    tca6408::Result<uint8_t> read_result = readRegisterResult(register_address);
    result.error = read_result.error;
    if (!read_result.ok()) {
      return result;
    }

    result.value = (read_result.value & bitMaskForPin_(pin)) != 0;
    return result;
  }
  tca6408::Result<void> updateRegisterBitResult(
    RegisterAddress register_address, uint8_t pin, bool value) {
    tca6408::Result<void> result;
    if (!pinValid_(pin)) {
      result.error = tca6408::I2cError::InvalidPin;
      last_i2c_error_ = result.error;
      return result;
    }

    tca6408::Result<uint8_t> read_result = readRegisterResult(register_address);
    result.error = read_result.error;
    if (!read_result.ok()) {
      return result;
    }

    uint8_t next_value = read_result.value;
    uint8_t pin_mask = bitMaskForPin_(pin);
    if (value) {
      next_value |= pin_mask;
    } else {
      next_value &= (uint8_t)~pin_mask;
    }

    return writeRegisterResult(register_address, next_value);
  }
  tca6408::I2cError
  endTransmissionToError_(uint8_t end_transmission_result) const {
    switch (end_transmission_result) {
      case 0:
        return tca6408::I2cError::None;
      case 1:
        return tca6408::I2cError::DataTooLong;
      case 2:
        return tca6408::I2cError::AddressNack;
      case 3:
        return tca6408::I2cError::DataNack;
      case 4:
      default:
        return tca6408::I2cError::OtherBusError;
    }
  }
};

#endif
