#ifndef RECEIVER_DISPLAY_h
#define RECEIVER_DISPLAY_h

#include <Arduino.h>
#include "U8g2lib.h"
#include "Modelisme.h"

#define SCREEN_1306_6432

class ReceiverDisplay {
  private:
    uint32_t currentTime;
    uint32_t displayTime;
    uint32_t refreshTime;
    int nbrdisp;
    int lastdisp;
    #if defined(SCREEN_1306_6432)
        U8G2_SSD1306_64X32_1F_F_HW_I2C* u8g2;
        #define Logo_challenger_width 63
        #define Logo_challenger_height 16
        #define Logo_challenger_length 128
        const uint8_t logo[Logo_challenger_length] = {\
          0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x01, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x09, 0xc0, 0x06, 0x00, 0x00, 0x00,\
          0x00, 0xc3, 0x0c, 0x40, 0x01, 0x00, 0x00, 0x00, 0xc0, 0x61, 0x06, 0xa0,\
          0x02, 0x00, 0x00, 0x00, 0x60, 0x00, 0x03, 0x58, 0x01, 0x00, 0x00, 0x00,\
          0x18, 0x80, 0xc5, 0xed, 0x02, 0x00, 0x00, 0x00, 0x0c, 0xc0, 0xa7, 0xb6,\
          0xea, 0x0c, 0x3b, 0x00, 0x06, 0xc0, 0x57, 0xd2, 0xfe, 0x96, 0x3a, 0x00,\
          0x07, 0x68, 0xfa, 0xcf, 0x7e, 0xd9, 0x1d, 0x00, 0xff, 0x27, 0x7f, 0xfb,\
          0xef, 0xfb, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    #else
        U8G2_SSD1306_128x64_1F_F_HW_I2C* u8g2;
        #define Logo_challenger_width 128
        #define Logo_challenger_height 40
        #define Logo_challenger_length 640
        const uint8_t logo[Logo_challenger_length+1] = {\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,\
          0x1c, 0x0e, 0x00, 0xc0, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x0f, 0x0e, 0x07, 0x00, 0xe0, 0x79, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x8e, 0x07, 0x00, 0x98,\
          0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,\
          0xc7, 0x03, 0x00, 0x48, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x78, 0xc0, 0xc3, 0x01, 0x00, 0x24, 0x19, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xc0, 0xe1, 0x01, 0x00, 0x92,\
          0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0xc0,\
          0xf0, 0x00, 0x00, 0x49, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0xc0, 0x03, 0x00, 0x78, 0x00, 0x80, 0x64, 0x03, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0xb8, 0x01, 0xc3, 0xb3,\
          0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00,\
          0xdc, 0xc1, 0xc7, 0xf9, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x3c, 0x00, 0x00, 0xee, 0xe1, 0xe7, 0x38, 0x0f, 0x00, 0x00, 0x80,\
          0x03, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0xff, 0x30, 0x73, 0x9c,\
          0xcd, 0xf9, 0x70, 0xc0, 0xe6, 0x03, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00,\
          0xef, 0x98, 0x39, 0xce, 0xec, 0x7f, 0x3c, 0x61, 0xf3, 0x03, 0x00, 0x00,\
          0xc0, 0x07, 0x00, 0x80, 0x67, 0xcc, 0x1c, 0xc7, 0xf7, 0x37, 0x87, 0xb3,\
          0xfb, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0xc6, 0x31, 0xee, 0xde, 0xe3,\
          0xb9, 0xbb, 0xe3, 0xf9, 0xec, 0x00, 0x00, 0x00, 0xe0, 0x8f, 0xff, 0xe7,\
          0xf9, 0xff, 0xff, 0xf3, 0xdd, 0xd9, 0xfd, 0x7c, 0x77, 0x00, 0x00, 0x00,\
          0xe0, 0xff, 0xff, 0xe3, 0xf8, 0xff, 0xbd, 0xff, 0xef, 0xfc, 0xef, 0xff,\
          0xfb, 0x7f, 0x00, 0x00, 0x80, 0xff, 0x3f, 0x70, 0x78, 0xef, 0x9c, 0xef,\
          0x03, 0xbc, 0xf3, 0xff, 0xf8, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
          0x00, 0x00, 0x00, 0x00 };
    #endif
    Receiver* rec;
    LightManager* lm;

    void _drawServo(int, int);
    void _drawScreenServos(int);
    void _drawScreen0();
    void _drawScreen1();
    void _drawScreen2();
    void _drawScreen3();
    void _drawScreen4();
    void _drawScreen5();

  public:
    String modelName;
    int max_channels;
    int speed_channel;

    ReceiverDisplay(String, int);
    void setup(Receiver*, LightManager*);
    void loop();

};

#endif