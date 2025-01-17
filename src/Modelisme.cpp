/*
  Mpodelisme.cpp - Various usefull libs for PWM.
  See .h for license details
*/

#include "Arduino.h"
#include "Modelisme.h"

void Receiver::setup()
{
  sBus.begin();
  max_channels=24;
}

void Receiver::setup(int max_channel)
{
  sBus.begin();
  this->max_channels = max_channel;
}

int Receiver::read()
{
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateChannels();
    sBus.toChannels = 0;

    for (int x=0; x<max_channels; x++) {
      channels[x].when = millis();
      channels[x].pwmvalue = sBus.channels[x];
      channels[x].angle = channels[x].pwmToDeg();
    };
    for (int i=0; i<max_channels; i++) {
      if (sBus.failsafe_status==0) {
        if (!servos[i].attached()) {servos[i].attach(servoPins[i]);};
        servos[i].write(channels[servoChannels[i]].angle);
      } else {
        if (servos[i].attached()) {servos[i].detach();};
      }
    }
    return 1;
  };
  return 0;
}

ReceiverCanal::ReceiverCanal()
{
  when=0;
  pwmvalue=0;
  angle=90;
}

int ReceiverCanal::pwmToDeg(){
  return map(pwmvalue, PWMMin, PWMMax, AngleMin, AngleMax);
};

LightManager::LightManager(){
  _setup();
};

void LightManager::setup(int rWarnPIN, int lWarnPIN, int brakePIN, int rearPIN, int lightsPIN, int highlightsPIN, int turningWarnPIN){
  if (rWarnPIN>0){
    rwPIN = rWarnPIN;
    pinMode(rwPIN, OUTPUT);
  };
  if (lWarnPIN>0){
    lwPIN = lWarnPIN;
    pinMode(lwPIN, OUTPUT);
  };
  if (brakePIN>0){
    bPIN = brakePIN;
    pinMode(bPIN, OUTPUT);
  };
  if (rearPIN>0){
    rPIN = rearPIN;
    pinMode(rPIN, OUTPUT);
  };
  if (lightsPIN>0){
    lPIN = lightsPIN;
    pinMode(lPIN, OUTPUT);
  };
  if (highlightsPIN>0){
    hlPIN = highlightsPIN;
    pinMode(hlPIN, OUTPUT);
  };
  if (turningWarnPIN>0){
    twPIN = turningWarnPIN;
    pinMode(twPIN, OUTPUT);
  };
};

void LightManager::_checkLights(int lightAngle, int throttleAngle, int steerAngle) {
  lightHistory.manageTheHistory(lightAngle);
  throttleHistory.manageTheHistory(throttleAngle);
  steerHistory.manageTheHistory(steerAngle);

  if (throttleAngle < 80)
    rear = true;
  if (throttleAngle > 95)
    rear = false;

  //Check lightHistoryCanal for a pattern
  //First, check if we reached the timing
  unsigned long actualTime = millis();
  //Max 3s, and at least one hit / 1.3sec
  if (lightHistory.history[0].timing>0 && (actualTime-lightHistory.getMaxHistoryTime()>3000 || actualTime-lightHistory.history[0].timing>1300)) {
    int state=lightHistory.getHistoryState();
    int pattern=lightHistory.getMaxHistoryLength();
    if (state!=0 && pattern>0){
      //Check the pattern
      if (state==1 && pattern==1 && not warnings){
        rWarn=not rWarn;
        if (rWarn) lWarn=false;
      } else if (state==-1 && pattern==1 && not warnings){
        lWarn=not lWarn;
        if (lWarn) rWarn=false;
      } else if (state==1 && pattern==2){
        warnings=not warnings;
        if (warnings){
          rWarn=false;
          lWarn=false;
        }
      } else if (state==-1 && pattern==2){
        lightWarn = true;
        lightWarnTime = millis()+1000;
      } else if (state==1 && pattern==3){
        turningWarn=not turningWarn;
      } else if (state==-1 && pattern==3){
        lights=not lights;
      } else if (state==-1 && pattern==4){
        highlights=not highlights;
      };
    };
    lightHistory.resetHistory();
  };

  //Check steerHistory for stoping warnings
    if (lWarn){
      if (steerHistory.history[0].angle>85 && steerHistory.getMinAngle()<85){
        lWarn = false;
      };
    };
    if (rWarn){
      if (steerHistory.history[0].angle<95 && steerHistory.getMaxAngle()>95){
        rWarn = false;
      };
    };

   //Check time for light Warning;
   if (lightWarn && actualTime>lightWarnTime) {
       lightWarn = false;
   }
  _blinking();
};

void LightManager::checkLights(int lightAngle, int throttleAngle, int steerAngle) {
  _checkLights(lightAngle, throttleAngle, steerAngle);
  _breaking();
  powerLights();
};

void LightManager::checkLights(int lightAngle, int throttleAngle, int steerAngle, int throttleAngle2) {
  throttleHistory2.manageTheHistory(throttleAngle2);
  _checkLights(lightAngle, throttleAngle, steerAngle);
  _breaking_dual();
  powerLights();
};

void LightManager::_setup(){
  blinktime = 0;
  rWarn = false;
  lWarn = false;
  brake = false;
  rear = false;
  lights = false;
  highlights = false;
  warnings = false;
  turningWarn = false;
  lightWarn = false;
  blinkstate = false;
  rwPIN = 0;
  lwPIN = 0;
  bPIN = 0;
  rPIN = 0;
  lPIN = 0;
  hlPIN = 0;
  twPIN = 0;
  lightWarnTime = 0;
};

void LightManager::_blinking() {
  //For the warnings ;-)
  if (millis()-blinktime >= 750) {
    if (blinkstate) {
      blinkstate=false;
    } else {
      blinkstate=true;
    }
    blinktime = millis();
  };
};

void LightManager::powerLights(){
  if (rwPIN>0){
    if ((rWarn || warnings) && blinkstate){
        digitalWrite(rwPIN, HIGH);
    } else {
        digitalWrite(rwPIN, LOW);
    };
  };
  if (lwPIN>0){
    if ((lWarn || warnings) && blinkstate){
      digitalWrite(lwPIN, HIGH);
    } else {
      digitalWrite(lwPIN, LOW);
    };
  };
  if (bPIN>0){
    if (brake){
      digitalWrite(bPIN, HIGH);
    } else {
      digitalWrite(bPIN, LOW);
    };
  };
  if (rPIN>0){
    if (rear){
      digitalWrite(rPIN, HIGH);
    } else {
      digitalWrite(rPIN, LOW);
    };
  };
  if (lPIN>0){
    if (lights || lightWarn){
      digitalWrite(lPIN, HIGH);
    } else {
      digitalWrite(lPIN, LOW);
    };
  };
  if (hlPIN>0){
    if (highlights || lightWarn){
      digitalWrite(hlPIN, HIGH);
    } else {
      digitalWrite(hlPIN, LOW);
    };
  };
  if (twPIN>0){
    if (turningWarn){
      digitalWrite(twPIN, HIGH);
    } else {
      digitalWrite(twPIN, LOW);
    };
  };
};

void LightManager::_breaking(){
  if (brake) {
    //Check if we accelerate to turn off th light
    if (throttleHistory.history[0].angle>98 && throttleHistory.isIncreasing()){
      //Accelerate Forward
      brake = false;
    };
    if (throttleHistory.history[0].angle<82 && throttleHistory.idDecreasing()){
      //Accelerate Backward
      brake = false;
    };
  } else {
    //Check if we decelerate to ligths up the led
    if (throttleHistory.history[0].angle>98 && throttleHistory.idDecreasing()){
      //Decelerate Forward
      brake = true;
    };
    if (throttleHistory.history[0].angle<82 && throttleHistory.isIncreasing()){
      //Decelerate Backward
      brake = true;
    };
  };
  if (brake) {
    if (throttleHistory.history[0].angle>87 && throttleHistory.history[0].angle<93 && throttleHistory.isAllEqual()){
      //Doesn't move
      brake = false;
    }
  }
};

void LightManager::_breaking_dual(){
  if (brake) {
    //Check if we accelerate to turn off th light
    if (throttleHistory.history[0].angle>98 && throttleHistory.isIncreasing() && throttleHistory2.isIncreasing()){
      //Accelerate Forward
      brake = false;
    };
    if (throttleHistory.history[0].angle<82 && throttleHistory.idDecreasing() && throttleHistory2.idDecreasing()){
      //Accelerate Backward
      brake = false;
    };
  } else {
    //Check if we decelerate to ligths up the led
    if (throttleHistory.history[0].angle>98 && throttleHistory.idDecreasing() && throttleHistory2.idDecreasing()){
      //Decelerate Forward
      brake = true;
    };
    if (throttleHistory.history[0].angle<82 && throttleHistory.isIncreasing() && throttleHistory2.isIncreasing()){
      //Decelerate Backward
      brake = true;
    };
  };
  if (brake) {
    if (throttleHistory.history[0].angle>87 && throttleHistory.history[0].angle<93 && throttleHistory.isAllEqual()){
      //Doesn't move
      brake = false;
    }
  }
};

int LightManagerHistory::canalToHighLow(int angle) {
  //Check if a canal is high, low or neutral, to help managing history of ligth canal
  if (angle<=0 || angle>=180){
    return 0;
  } else if (angle>115){
    return 1;
  } else if (angle<75){
    return -1;
  } else {
    return 0;
  };
};


void LightManagerHistory::manageTheHistory(int angle) {
   //Manage history of the canal
   //Check sequences of type 3x High, 2x Low, ... to manage multiple functions on 1 canal
   int state = canalToHighLow(angle);
   if (state!=history[0].state){
      //Do not store the middle values
      if (history[0].state!=0) {
       for (int i=MAX_LM_HISTORY-1;i>0;i--){
         history[i].state=history[i-1].state;
         history[i].timing=history[i-1].timing;
       };
      };
     history[0].state = state;
     history[0].timing = millis();
   };
};

void LightManagerHistory::resetHistory(){
  for (int i=0;i<MAX_LM_HISTORY;i++){
    history[i].state=0;
    history[i].timing=0;
  }
};

unsigned long LightManagerHistory::getMaxHistoryTime(){
  unsigned long res=history[0].timing;
  for (int i=1;i<MAX_LM_HISTORY;i++){
    if (history[i].timing>0 and history[i].timing<res){
      res=history[i].timing;
    }
  };
  return res;
};

int LightManagerHistory::getMaxHistoryLength(){
  int res=0;
  bool toContinue=true;

  //If history[0]==0, means switch is neutral, need to check from pos 1
  int startpos=0;
  if (history[0].state==0){startpos=1;};

  if (history[startpos].state!=0){
    for (int i=startpos;i<MAX_LM_HISTORY && toContinue;i++){
      if (history[i].state==history[startpos].state && history[i].timing>0){
        res++;
      }
    }
  }
  return res;
};

int LightManagerHistory::getHistoryState(){
  if (history[0].state==0) {
    return history[1].state;
  };
  return history[0].state;
};

LightManagerHistoryLine::LightManagerHistoryLine()
{
  state=0;
  timing=0;
}

ChannelHistory::ChannelHistory() {
    int i=1;
};

void ChannelHistory::manageTheHistory(int angle) {
   //Manage history of the canal
   for (int i=MAX_LM_HISTORY;i>0;i--){
     history[i].angle=history[i-1].angle;
     history[i].timing=history[i-1].timing;
   };
   history[0].angle = angle;
   history[0].timing = millis();
}

int ChannelHistory::getMinAngle(){
    int res = history[0].angle;
    for (int i=1;i<MAX_LM_HISTORY;i++){
        if (history[i].angle<res){
          res = history[i].angle;
        };
    };
    return res;
};

int ChannelHistory::getMaxAngle(){
    int res = history[0].angle;
    for (int i=1;i<MAX_LM_HISTORY;i++){
        if (history[i].angle>res){
          res = history[i].angle;
        };
    };
    return res;
};

bool ChannelHistory::isIncreasing(){
    bool toReturn=true;
    for (int i=1;i<MAX_LM_HISTORY;i++){
      if (history[i-1].angle<history[i].angle){
        toReturn=false;
      };
    };
    if (toReturn && !isAllEqual()) {
      return true;
    }
    return false;
};

bool ChannelHistory::idDecreasing(){
    bool toReturn=true;

    for (int i=1;i<MAX_LM_HISTORY;i++){
      if (history[i-1].angle>history[i].angle){
        toReturn=false;
      };
    };
    if (toReturn && !isAllEqual()) {
      return true;
    }
    return false;
};

bool ChannelHistory::isAllEqual(){
  bool allequal=true;
  //Ensure not everything is equal
  for (int i=1;i<MAX_LM_HISTORY;i++){
    if (history[i-1].angle!=history[i].angle){
      allequal=false;
    };
  };
  return allequal;
};

ChannelHistoryLine::ChannelHistoryLine()
{
  angle=0;
  timing=0;
};
