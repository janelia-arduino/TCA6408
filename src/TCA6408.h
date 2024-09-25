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


class TCA6408
{
public:
  TCA6408();

  enum DeviceAddress
  {
    DEVICE_ADDRESS_0=0x20,
    DEVICE_ADDRESS_1=0x21,
  };

  void setup(TwoWire & wire,
    DeviceAddress device_address);
  uint8_t readInputs();
  void setResetPin(pin_size_t reset_pin);
private:
  TwoWire * wire_ptr_;
  DeviceAddress device_address_;
  pin_size_t reset_pin_;
  const static pin_size_t NO_PIN = 255;
};

#endif
