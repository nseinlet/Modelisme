#include "FrSky_SBus.h"

void FRSKY_SBUS::begin(){
	uint8_t loc_sbusData[SBUS_DATA_SIZE+1] = {
	  0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};

  	port.begin(BAUDRATE);

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
	sbus_passthrough = 1;
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
uint8_t FRSKY_SBUS::DigiChannel(uint8_t ch) {
  // Read digital channel data
  if ((ch>0) && (ch<=2)) {
    return channels[15+ch];
  }
  else{
    return 0;
  }
}
void FRSKY_SBUS::Servo(uint8_t ch, int16_t position) {
  // Set servo position
  if ((ch>0)&&(ch<=CHANNEL_SIZE-2)) {
    if (position>2048) {
      position=2048;
    }
    servos[ch-1] = position;
  }
}
void FRSKY_SBUS::DigiServo(uint8_t ch, uint8_t position) {
  // Set digital servo position
  if ((ch>0) && (ch<=2)) {
    if (position>1) {
      position=1;
    }
    servos[15+ch] = position;
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
void FRSKY_SBUS::FeedLine(void){
  if (port.available() > SBUS_DATA_SIZE){
    while(port.available() > 0){
      inData = port.read();
      switch (feedState){
      case 0:
        if (inData != 0x0f){
          while(port.available() > 0){//read the contents of in buffer this should resync the transmission
            inData = port.read();
          }
          return;
        }
        else{
          bufferIndex = 0;
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
          if (inBuffer[0]==0x0f && inBuffer[SBUS_DATA_SIZE] == 0x00){
            memcpy(sbusData,inBuffer,SBUS_DATA_SIZE+1);
            toChannels = 1;
          }
        }
        break;
      }
    }
  }
}
