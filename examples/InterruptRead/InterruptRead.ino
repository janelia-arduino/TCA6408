#include <TCA6408.h>

#if !defined(ARDUINO_ARCH_RP2040)
TwoWire &wire = Wire;
#else
TwoWire &wire = Wire1;
uint8_t SDA_PIN = 26;
uint8_t SCL_PIN = 27;
#endif

const TCA6408::DeviceAddress DEVICE_ADDRESS = TCA6408::DEVICE_ADDRESS_0;
uint8_t RESET_PIN = 0;
uint8_t INTERRUPT_PIN = 1;

const uint32_t SERIAL_BAUD_RATE = 115200;
const uint16_t LOOP_DELAY = 2000;

// Instantiate TCA6408
TCA6408 io_expander;
volatile bool input_changed;

void interruptCallback() { input_changed = true; }

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

#if defined(ARDUINO_ARCH_RP2040)
  wire.setSDA(SDA_PIN);
  wire.setSCL(SCL_PIN);
#endif
  wire.begin();

  io_expander.setup(wire, DEVICE_ADDRESS);
  io_expander.setResetPin(RESET_PIN);
  input_changed = false;

  io_expander.attachInterrupt(INTERRUPT_PIN, interruptCallback);
}

void loop() {
  if (input_changed) {
    noInterrupts();
    input_changed = false;
    interrupts();

    uint8_t input_register = io_expander.readInputRegister();
    Serial.print("input_register: 0b");
    Serial.print(input_register, BIN);
    Serial.print(", ");
    Serial.println(input_register);
    Serial.print("last_i2c_error: ");
    Serial.println((uint8_t)io_expander.getLastI2cError());
    Serial.println("-------------------------------------");
  }
}
