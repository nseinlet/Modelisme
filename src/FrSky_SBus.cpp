#include "FrSky_SBus.h"
#if defined(ARDUINO_AVR_NANO_EVERY)
// #include "NanoEverySerial.h"
#endif

void FRSKY_SBUS::begin(){
	uint8_t loc_sbusData[SBUS_DATA_SIZE+1] = {
    #if defined(ACCESS_24)
      0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
    #else
      0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
    #endif
    // #if defined(ARDUINO_AVR_NANO_EVERY)
    //  NanoEverySerial1.begin();
    // #else
  	  port.begin(BAUDRATE, SERIAL_8E2);
    // #endif

    #if defined(SBUS_DEBUG)
      readErrors = 0;
    #endif

	//memcpy(sbusData,loc_sbusData,SBUS_DATA_SIZE+1);
  for (int i=0;i<CHANNEL_SIZE;i++){
    channels[i] = 1023;
    servos[i] = 1023;
    if (i>=CHANNEL_SIZE-2) {
      channels[i] = 0;
      servos[i] = 0;
    };
  }
	failsafe_status = SBUS_NO_SIGNAL;
	toChannels = 0;
	bufferIndex=0;
	feedState = 0;
}

int16_t FRSKY_SBUS::Channel(uint8_t ch) {
  // Read channel data
  if ((ch>0)&&(ch<=CHANNEL_SIZE-2)){
    return channels[ch-1];
  }
  else{
    return 1023;
  }
}

uint8_t FRSKY_SBUS::Failsafe(void) {
  return failsafe_status;
}

void FRSKY_SBUS::UpdateChannels(void) {

  channels[0]  = ((sbusData[1]|sbusData[2]<< 8) & 0x07FF);
  channels[1]  = ((sbusData[2]>>3|sbusData[3]<<5) & 0x07FF);
  channels[2]  = ((sbusData[3]>>6|sbusData[4]<<2|sbusData[5]<<10) & 0x07FF);
  channels[3]  = ((sbusData[5]>>1|sbusData[6]<<7) & 0x07FF);
  channels[4]  = ((sbusData[6]>>4|sbusData[7]<<4) & 0x07FF);
  channels[5]  = ((sbusData[7]>>7|sbusData[8]<<1|sbusData[9]<<9) & 0x07FF);
  channels[6]  = ((sbusData[9]>>2|sbusData[10]<<6) & 0x07FF);
  channels[7]  = ((sbusData[10]>>5|sbusData[11]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
  #ifdef ALL_CHANNELS || ACCESS_24
  channels[8]  = ((sbusData[12]|sbusData[13]<< 8) & 0x07FF);
  channels[9]  = ((sbusData[13]>>3|sbusData[14]<<5) & 0x07FF);
  channels[10] = ((sbusData[14]>>6|sbusData[15]<<2|sbusData[16]<<10) & 0x07FF);
  channels[11] = ((sbusData[16]>>1|sbusData[17]<<7) & 0x07FF);
  channels[12] = ((sbusData[17]>>4|sbusData[18]<<4) & 0x07FF);
  channels[13] = ((sbusData[18]>>7|sbusData[19]<<1|sbusData[20]<<9) & 0x07FF);
  channels[14] = ((sbusData[20]>>2|sbusData[21]<<6) & 0x07FF);
  channels[15] = ((sbusData[21]>>5|sbusData[22]<<3) & 0x07FF);
  #endif
  #ifdef ACCESS_24
  channels[16] = ((sbusData[23]|sbusData[24]<< 8) & 0x07FF);
  channels[17] = ((sbusData[24]>>3|sbusData[25]<<5) & 0x07FF);
  channels[18] = ((sbusData[25]>>6|sbusData[26]<<2|sbusData[27]<<10) & 0x07FF);
  channels[19] = ((sbusData[27]>>1|sbusData[28]<<7) & 0x07FF);
  channels[20] = ((sbusData[28]>>4|sbusData[29]<<4) & 0x07FF);
  channels[21] = ((sbusData[29]>>7|sbusData[30]<<1|sbusData[31]<<9) & 0x07FF);
  channels[22] = ((sbusData[31]>>2|sbusData[32]<<6) & 0x07FF);
  channels[23] = ((sbusData[32]>>5|sbusData[33]<<3) & 0x07FF);
  #endif

  // Failsafe
  failsafe_status = SBUS_SIGNAL_OK;
  if (sbusData[SBUS_DATA_STATUS] & (1<<2)) {
    failsafe_status = SBUS_SIGNAL_LOST;
  }
  if (sbusData[SBUS_DATA_STATUS] & (1<<3)) {
    failsafe_status = SBUS_SIGNAL_FAILSAFE;
  }

}

// #if defined(ARDUINO_AVR_NANO_EVERY)

// void FRSKY_SBUS::readSerial(void){
// }

// void FRSKY_SBUS::FeedLine(void){
//   if (NanoEverySerial1.read()){
//     bufferIndex = HARDWARE_SERIAL_READ_SIZE;

//     #if defined SBUS_DEBUG
//       //memcpy(dbg1, NanoEverySerial1.buffer, bufferIndex);
//     #endif

//     while(bufferIndex>SBUS_DATA_SIZE){
//       if (NanoEverySerial1.buffer[bufferIndex-SBUS_DATA_SIZE] == SBUS_FRAME_BEGIN && NanoEverySerial1.buffer[bufferIndex] == SBUS_FRAME_END){
//         memcpy(sbusData, NanoEverySerial1.buffer+bufferIndex-SBUS_DATA_SIZE, SBUS_DATA_SIZE+1);

//         #if defined SBUS_DEBUG
//           lastread = millis();
//         #endif
//         toChannels = 1;
//         return;
//       }
//       bufferIndex--;
//     }
//   #if defined SBUS_DEBUG
//   } else {
//       readErrors++;
//   #endif
//   };
// }
// #else
void FRSKY_SBUS::readSerial(void){
  prevData = inData;
  inData = port.read();
}

void FRSKY_SBUS::FeedLine(void){
  if (port.available() > SBUS_DATA_SIZE){
    while(port.available() > 0){
      readSerial();
      switch (feedState){
      case 0:
        bufferIndex = 0;
        while(port.available() > 0 && inData != SBUS_FRAME_BEGIN && prevData != SBUS_FRAME_END){
          //read the contents of in buffer this should resync the transmission
          readSerial();
        }
        if (inData == SBUS_FRAME_BEGIN){
          inBuffer[bufferIndex] = inData;
          inBuffer[SBUS_DATA_SIZE] = 0xff;
          feedState = 1;
        }
        break;
      case 1:
        bufferIndex ++;
        inBuffer[bufferIndex] = inData;
        if (bufferIndex < SBUS_DATA_SIZE && port.available() == 0){
          feedState = 0;
        }
        if (bufferIndex == SBUS_DATA_SIZE){
          feedState = 0;
          // Perform some checks:
          // Last byte must be 0
          // Second last byte must be 0, 4, 8 or 12, aka the SBus status byte 
          // There' s nothing more I can do to ensure it' s a SBus packet and not some crappy noise.
          if (inBuffer[SBUS_DATA_SIZE] == SBUS_FRAME_END && (inBuffer[SBUS_DATA_SIZE-1]==0 || inBuffer[SBUS_DATA_SIZE-1]==4 || inBuffer[SBUS_DATA_SIZE-1]==8 || inBuffer[SBUS_DATA_SIZE-1]==12)){
            memcpy(sbusData,inBuffer,SBUS_DATA_SIZE+1);
            #if defined SBUS_DEBUG
            lastread = millis();
            #endif
            toChannels = 1;
            return;
          #if defined SBUS_DEBUG
          } else {
              readErrors++;
          #endif
          };
        }
        break;
      }
    }
  }
}
// #endif

#if defined SBUS_DEBUG
void FRSKY_SBUS::debug(Stream& serialPort){
  // serialPort.print("Failsafe: ");
  // serialPort.println(failsafe_status);
  // for (int i=0;i<CHANNEL_SIZE;i++){
  //   serialPort.print("\t");
  //   serialPort.print(channels[i]);
  // }
  // serialPort.println();

  //serialPort.print(port.available());
  //serialPort.print('\t');
  //serialPort.print(SBUS_DATA_SIZE);
  //serialPort.print('\t');
  serialPort.print(lastread);
  serialPort.print('\t');
  serialPort.print(readErrors);
  serialPort.print('\t');
  serialPort.print(bufferIndex);
  serialPort.println('\t');
  // serialPort.print("SerialBuffer: ");
  // for (int i=0;i<MAX_DATA_SIZE;i++){
  //   serialPort.print(i);
  //   serialPort.print(':');
  //   serialPort.print(dbg1[i],HEX);
  //   serialPort.print('\t');
  // };
  // serialPort.println('\t');
  serialPort.print("SBUSData: ");
  for (int i=0;i<SBUS_DATA_SIZE+1;i++){
    serialPort.print(i);
    serialPort.print(':');
    serialPort.print(sbusData[i],HEX);
    serialPort.print('\t');
  };
  serialPort.println('\t');
  // serialPort.print(USART1.STATUS & USART_RXCIF_bm);
  // serialPort.print('\t');
  // serialPort.print(USART1.STATUS);
  // serialPort.print('\t');
  // serialPort.print(USART_RXCIF_bm);
  // serialPort.println('\t');
}
#endif
