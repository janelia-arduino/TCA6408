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
}

void TCA6408::setup(TwoWire & wire,
  DeviceAddress device_address)
{
  wire_ptr_ = &wire;
  device_address_ = device_address;

  wire_ptr_->begin();
#if defined(WIRE_HAS_TIMEOUT)
  wire_ptr_->setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif
}

uint8_t TCA6408::readInputs()
{
  if (wire_ptr_ == nullptr)
  {
    return 0;
  }
  wire_ptr_->beginTransmission(device_address_);
  wire_ptr_->write(0x0);
  byte error = wire_ptr_->endTransmission();
  if (error)
  {
    Serial.println("Error occurred when writing");
    if (error == 5)
      Serial.println("It was a timeout");
  }

  delay(100);

#if defined(WIRE_HAS_TIMEOUT)
  wire_ptr_->clearWireTimeoutFlag();
#endif
  byte len = wire_ptr_->requestFrom(device_address_, 1); // request 1 byte
  if (len == 0)
  {
    Serial.println("Error occurred when reading");
#if defined(WIRE_HAS_TIMEOUT)
    if (wire_ptr_->getWireTimeoutFlag())
      Serial.println("It was a timeout");
#endif
  }

  uint8_t inputs = wire_ptr_->read();
  return inputs;
}

void TCA6408::setResetPin(pin_size_t reset_pin)
{
  reset_pin_ = reset_pin;
  pinMode(reset_pin, OUTPUT);
  digitalWrite(reset_pin, HIGH);
}
