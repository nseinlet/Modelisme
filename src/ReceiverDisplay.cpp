/*Copyright (C) 2024  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/

#include "Arduino.h"
#include "ReceiverDisplay.h"
#include <math.h>


ReceiverDisplay::ReceiverDisplay(String modelName, int maxChannels) {
    displayTime = 0;
    refreshTime = 0;
    nbrdisp = 0;
    this->modelName = modelName;
    this->max_channels = maxChannels;
    #if defined(SCREEN_1306_6432)
        u8g2 = new U8G2_SSD1306_64X32_1F_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    #else
        u8g2 = new U8G2_SSD1306_128x64_1F_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    #endif
}

void ReceiverDisplay::setup(Receiver* rec, LightManager* lm){
    u8g2->begin();
    this->rec = rec;
    this->lm = lm;
    //uint8_t loc_logo[Logo_challenger_length+1] = Logo_challenger_bits;
    //memcpy(loc_logo, logo, Logo_challenger_length);
}

void ReceiverDisplay::loop(){
    currentTime = millis();
    if(currentTime > displayTime)
    {
        displayTime = currentTime + 5000;
        nbrdisp = nbrdisp + 1;
        if (nbrdisp>7) {
          nbrdisp=0;
        }
    }
    if(currentTime > refreshTime) 
    {
        refreshTime = currentTime + 1000;
        u8g2->clearBuffer();
        //if (nbrdisp==0) {
          _drawScreen0();
        // } else if (nbrdisp==1){
        //   _drawScreen1();
        // } else if (nbrdisp==2){
        //   _drawScreen2();
        // } else if (nbrdisp==3){
        //   _drawScreen3();
        // } else if (nbrdisp==4){
        //   _drawScreen4();
        // } else {
        //   _drawScreen5();
        // };
        u8g2->sendBuffer();
    };
}

void ReceiverDisplay::_drawServo(int pos, int angle) {
    int draw_angle = angle-90;
    if (angle>90) {angle=90;};
    if (angle<-90) {angle=-90;};

    u8g2->drawLine(  pos,  1, 4+pos,  1);
    u8g2->drawLine(  pos, 23,   pos,  1);
    u8g2->drawLine(1+pos, 12, 1+pos, 12+(draw_angle/9));
    u8g2->drawLine(2+pos, 12, 2+pos, 12+(draw_angle/9));
    u8g2->drawLine(3+pos, 12, 3+pos, 12+(draw_angle/9));
    u8g2->drawLine(4+pos, 23, 4+pos,  1);
    u8g2->drawLine(  pos, 23, 4+pos, 23);
}

void ReceiverDisplay::_drawScreenServos(int first_channel) {
    int offset = 0;
    if (max_channels>first_channel){
        u8g2->setFont(u8g2_font_4x6_tf);
        for (int i=first_channel;i<(first_channel+8);i++) {
          _drawServo(2+(i-first_channel)*8, rec->channels[i].angle);
          if (i<9) {offset=3;} else {offset=0;}; 
          u8g2->drawStr(offset+(i-first_channel)*8, 32, String(i+1).c_str());
        };
          
    } else {
        _drawScreenServos(first_channel-8);
    }
}

void ReceiverDisplay::_drawScreen0() {
    //Just a logo :-)  
    u8g2->drawXBM( 2, 4, Logo_challenger_width, Logo_challenger_height, logo);
    u8g2->setFont(u8g2_font_5x7_tf);
    u8g2->drawStr( 17, 30, modelName.c_str());
};

void ReceiverDisplay::_drawScreen1() {
    _drawScreenServos(0);
};

void ReceiverDisplay::_drawScreen2() {
    _drawScreenServos(8);
};

void ReceiverDisplay::_drawScreen3() {
    _drawScreenServos(16);
};

void ReceiverDisplay::_drawScreen4() {
    // Lights
    u8g2->setFont(u8g2_font_cursor_tf);
    if ((lm->lWarn || lm->warnings) && lm->blinkstate) {
    u8g2->drawStr( 1, 4, "\x8f");
    };
    if ((lm->rWarn || lm->warnings) && lm->blinkstate) {
    u8g2->drawStr(63, 4, "\x91");
    };
    if (rec->channels[speed_channel].angle>85){
    u8g2->drawStr(28, 17, "\x93");
    };
    if (rec->channels[speed_channel].angle<95){
    u8g2->drawStr(38, 32, "\x8b");
    };
    if (lm->turningWarn) {
    u8g2->drawStr(53, 23, "\x53");  
    };
    if (lm->lights) {
    u8g2->drawStr(5, 22, "\xae");
    };
    if (lm->highlights || lm->lightWarn) {
    u8g2->drawStr(5, 22, "\xaf");
    };
};

void ReceiverDisplay::_drawScreen5() {
    //Power gauge
    u8g2->drawCircle(32, 32, 30, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
    u8g2->drawCircle(32, 32, 29, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
    u8g2->drawCircle(32, 32, 28, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
        
    int angle = abs(rec->channels[speed_channel].angle-90)*2;
    if (angle>175) {angle=175;};
    if (angle<30) {angle=30;};
        
    int endpointx = 32 - (int) (28 * cos((angle * 71) / 4068.0));
    int endpointy = 32 - (int) (28 * sin((angle * 71) / 4068.0));
    u8g2->drawLine(31, 32, endpointx, endpointy);
    u8g2->drawLine(32, 32, endpointx, endpointy);
    u8g2->drawLine(33, 32, endpointx, endpointy);

    for(angle=10;angle<180;angle+=25){
    int startpointx = 32 - (int) (22 * cos((angle * 71) / 4068.0));
    int startpointy = 32 - (int) (22 * sin((angle * 71) / 4068.0));
    endpointx = 32 - (int) (28 * cos((angle * 71) / 4068.0));
    endpointy = 32 - (int) (28 * sin((angle * 71) / 4068.0));

    u8g2->drawLine(startpointx, startpointy, endpointx, endpointy);
    }

    u8g2->setFont(u8g2_font_cursor_tf);
    if ((lm->lWarn || lm->warnings) && lm->blinkstate) {
    u8g2->drawStr( 1, 4, "\x8f");
    };
    if ((lm->rWarn || lm->warnings) && lm->blinkstate) {
    u8g2->drawStr(63, 4, "\x91");
    };
};

