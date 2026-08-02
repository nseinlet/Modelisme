#include <Arduino.h>

#define HARDWARE_SERIAL_READ_SIZE 64

class NanoEverySerial {
public:
    uint8_t buffer[HARDWARE_SERIAL_READ_SIZE];
    void begin();
    bool read();
private:
    bool _parityError;
    bool _available();
    uint8_t _read();
};

// Pre-instantiate the object for ease of use (like native Serial)
extern NanoEverySerial NanoEverySerial1;