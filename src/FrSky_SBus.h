/* Part of modelisme library

Original author : mikeshub: https://github.com/mikeshub/FUTABA_SBUS
*/

#ifndef FRSKY_SBUS_h
#define FRSKY_SBUS_h

#include <Arduino.h>

#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x03
#define SBUS_NO_SIGNAL          0x07
#define BAUDRATE 100000

#define SBUS_FRAME_BEGIN 0x0f
#define SBUS_FRAME_END 0x00
#define SBUS_DEBUG

#define ARBITRARY_DATA_SIZE 58
#define MAX_DATA_SIZE 64

#if defined(ARDUINO_AVR_NANO_EVERY)
  #define port Serial1
#else
  #define port Serial
#endif

#define ACCESS_24
#if defined(ACCESS_24)
  #define CHANNEL_SIZE 26
  #define SBUS_DATA_STATUS 34
  #define SBUS_DATA_SIZE 35
#else
  #define CHANNEL_SIZE 18
  #define SBUS_DATA_STATUS 23
  #define SBUS_DATA_SIZE 24
#endif

#define ALL_CHANNELS

class FRSKY_SBUS
{
	public:
		uint8_t sbusData[SBUS_DATA_SIZE+1];
		uint8_t serialBuffer[MAX_DATA_SIZE+1];
		int16_t channels[CHANNEL_SIZE];
		int16_t servos[CHANNEL_SIZE];
		uint8_t failsafe_status;
		int toChannels;
		void begin(void);
		int16_t Channel(uint8_t ch);
		uint8_t Failsafe(void);
		void UpdateChannels(void);
		void FeedLine(void);
		#if defined SBUS_DEBUG
		void debug(Stream& serialPort);
		uint8_t dbg1[MAX_DATA_SIZE+1];
		uint8_t dbg2[MAX_DATA_SIZE+1];
		uint8_t dbg3[MAX_DATA_SIZE+1];
		#endif
	private:
		uint8_t byte_in_sbus;
		uint8_t bit_in_sbus;
		uint8_t ch;
		uint8_t bit_in_channel;
		uint8_t bit_in_servo;
		uint8_t inBuffer[SBUS_DATA_SIZE+1];
		int bufferIndex;
		uint8_t inData;
		int feedState;
		uint8_t prevData;
		#if defined SBUS_DEBUG
			unsigned long lastread;
			unsigned long readErrors;
		#endif
		void readSerial(void);
};

#endif
