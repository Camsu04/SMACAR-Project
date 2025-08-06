/*!
 *@file LoRa_RECV_POLL.ino
 *@brief The LoRa receiving node is set with the node address of 4.
 *@details  
 *@copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [MaTing]
 *@version  V1.0
 *@date  2024-11-22
 *@https://github.com/DFRobot/DFRobot_LWNode
*/
#include <DFRobot_LWNode.h>

#define REGION_EU868
//#define REGION_US915
//#define REGION_CN470

#ifdef REGION_EU868
  #define FREQ  866100000
#elif defined(REGION_US915)
  #define FREQ    913900000
#elif defined(REGION_CN470)
  #define FREQ    470300000
#endif

DFRobot_LWNode_IIC node(4);

void setup( void ) {
    Serial.begin(115200);
    delay(5000);
    node.begin(/*communication IIC*/&Wire,/*debug UART*/&Serial);

    while(!node.setFreq(FREQ)){
        delay(2000);
        Serial.println("Failed to set Freq ");
    }

    while (!node.setEIRP(DBM16)) {
        delay(2000);
        Serial.println("Failed to set EIRP");
    }

    while (!node.setBW(125000)) {
        delay(2000);
        Serial.println("Failed to set BW");
    }
    
    while (!node.setSF(12)) {
        delay(2000);
        Serial.println("Failed to set SF");
    }
    
    if (!node.start()) {
        delay(2000);
        Serial.println("Failed to Start");
    }
}

void loop( void ){
    if(node.available())
    {
        Serial.print("poll data num: ");
        Serial.println(node.available());
        
        String data = node.readData();
        Serial.println(data);

        Serial.print("poll data num: ");
        Serial.println(node.available());
        
        Serial.print("SNR: ");
        Serial.println(node.readSNR());
        Serial.print("RSSI: ");
        Serial.println(node.readRSSI());
        Serial.print("FROM: ");
        Serial.println(node.readAddrFROM());
    }
    delay(100);
}
