#include <TCA6408.h>

#if !defined(ARDUINO_ARCH_RP2040)
TwoWire& wire = Wire;
#else
TwoWire& wire = Wire1;
uint8_t SDA_PIN = 26;
uint8_t SCL_PIN = 27;
#endif

const TCA6408::DeviceAddress DEVICE_ADDRESS = TCA6408::DEVICE_ADDRESS_0;
uint8_t RESET_PIN = 0;

const uint32_t SERIAL_BAUD_RATE = 115200;
const uint16_t LOOP_DELAY = 2000;

// Instantiate TCA6408
TCA6408 io_expander;
bool pins_inverted;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

#if defined(ARDUINO_ARCH_RP2040)
  wire.setSDA(SDA_PIN);
  wire.setSCL(SCL_PIN);
#endif
  wire.begin();

  io_expander.setup(wire, DEVICE_ADDRESS);
  io_expander.setResetPin(RESET_PIN);

  pins_inverted = false;
}

void loop() {
  uint8_t input_register = io_expander.readInputRegister();
  Serial.print("input_register: 0b");
  Serial.print(input_register, BIN);
  Serial.print(", ");
  Serial.println(input_register);
  Serial.print("last_i2c_error: ");
  Serial.println((uint8_t)io_expander.getLastI2cError());

  uint8_t polarity_inversion_register =
    io_expander.readPolarityInversionRegister();
  Serial.print("polarity_inversion_register: 0b");
  Serial.println(polarity_inversion_register, BIN);

  uint8_t configuration_register = io_expander.readConfigurationRegister();
  Serial.print("configuration_register: 0b");
  Serial.println(configuration_register, BIN);

  if (pins_inverted) {
    Serial.println("all pins polarity were inverted now switching");
    pins_inverted = false;
    io_expander.setAllPinsPolarityOriginal();
  } else {
    Serial.println("all pins polarity were original now switching");
    pins_inverted = true;
    io_expander.setAllPinsPolarityInverted();
  }

  Serial.println("-------------------------------------");
  delay(LOOP_DELAY);
}
