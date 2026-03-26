#include <TCA6408.h>

#if !defined(ARDUINO_ARCH_RP2040)
TwoWire & wire = Wire;
#else
TwoWire & wire = Wire1;
uint8_t SDA_PIN = 26;
uint8_t SCL_PIN = 27;
#endif

const TCA6408::DeviceAddress DEVICE_ADDRESS = TCA6408::DEVICE_ADDRESS_0;
const uint8_t INPUT_PIN = TCA6408::PIN_1;
const uint8_t OUTPUT_PIN = TCA6408::PIN_7;

const uint32_t SERIAL_BAUD_RATE = 115200;
const uint16_t LOOP_DELAY = 1000;

TCA6408 io_expander;
bool output_high;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

#if defined(ARDUINO_ARCH_RP2040)
  wire.setSDA(SDA_PIN);
  wire.setSCL(SCL_PIN);
#endif
  wire.begin();

  io_expander.setup(wire, DEVICE_ADDRESS);

  io_expander.setPinInput(INPUT_PIN);
  io_expander.setPinOutput(OUTPUT_PIN);
  io_expander.setPinLow(OUTPUT_PIN);
  output_high = false;
}

void loop()
{
  bool input_high = io_expander.readInputPin(INPUT_PIN);
  bool latched_high = io_expander.readOutputPin(OUTPUT_PIN);

  Serial.print("input_pin ");
  Serial.print(INPUT_PIN);
  Serial.print(": ");
  Serial.println(input_high);

  Serial.print("output_pin ");
  Serial.print(OUTPUT_PIN);
  Serial.print(" latch: ");
  Serial.println(latched_high);

  if (output_high)
  {
    io_expander.setPinLow(OUTPUT_PIN);
    output_high = false;
  }
  else
  {
    io_expander.setPinHigh(OUTPUT_PIN);
    output_high = true;
  }

  Serial.print("last_i2c_error: ");
  Serial.println((uint8_t)io_expander.getLastI2cError());
  Serial.println("-------------------------------------");
  delay(LOOP_DELAY);
}
