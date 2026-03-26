// ----------------------------------------------------------------------------
// TCA6408.cpp
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "TCA6408.h"


TCA6408::TCA6408()
{
  wire_ptr_ = nullptr;
  reset_pin_ = NO_PIN;
  last_i2c_error_ = tca6408::I2cError::NotInitialized;
}

void TCA6408::setup(TwoWire & wire,
  DeviceAddress device_address)
{
  wire_ptr_ = &wire;
  device_address_ = device_address;
#if defined(WIRE_HAS_TIMEOUT)
  wire_ptr_->setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif
  last_i2c_error_ = tca6408::I2cError::None;
}

uint8_t TCA6408::readRegister(RegisterAddress register_address)
{
  return readRegisterResult(register_address).value;
}

void TCA6408::writeRegister(RegisterAddress register_address, uint8_t data)
{
  writeRegisterResult(register_address, data);
}

uint8_t TCA6408::readInputRegister()
{
  return readRegister(INPUT_PORT);
}

uint8_t TCA6408::readOutputRegister()
{
  return readRegister(OUTPUT_PORT);
}

uint8_t TCA6408::readPolarityInversionRegister()
{
  return readRegister(POLARITY_INVERSION);
}

void TCA6408::writePolarityInversionRegister(uint8_t data)
{
  writeRegister(POLARITY_INVERSION, data);
}

void TCA6408::setAllPinsPolarityOriginal()
{
  writeRegister(POLARITY_INVERSION, 0x00);
}

void TCA6408::setAllPinsPolarityInverted()
{
  writeRegister(POLARITY_INVERSION, 0xFF);
}

uint8_t TCA6408::readConfigurationRegister()
{
  return readRegister(CONFIGURATION);
}

void TCA6408::writeConfigurationRegister(uint8_t data)
{
  writeRegister(CONFIGURATION, data);
}

void TCA6408::setAllPinsInput()
{
  writeRegister(CONFIGURATION, 0xFF);
}

void TCA6408::setAllPinsOutput()
{
  writeRegister(CONFIGURATION, 0x00);
}

void TCA6408::writeOutputRegister(uint8_t data)
{
  writeRegister(OUTPUT_PORT, data);
}

void TCA6408::setResetPin(uint8_t reset_pin)
{
  reset_pin_ = reset_pin;
  pinMode(reset_pin, OUTPUT);
  digitalWrite(reset_pin, HIGH);
}

void TCA6408::reset()
{
  if (reset_pin_ == NO_PIN)
  {
    return;
  }

  digitalWrite(reset_pin_, LOW);
  delayMicroseconds(1);
  digitalWrite(reset_pin_, HIGH);
}

void TCA6408::attachInterrupt(uint8_t interrupt_pin, voidFuncPtr callback)
{
  ::attachInterrupt(digitalPinToInterrupt(interrupt_pin), callback, FALLING);
}
