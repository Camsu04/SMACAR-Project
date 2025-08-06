/*!
 *@file LoRa_UART.ino
 *@brief The LoRa transmitting node is set with the address 3 and sends data to the node with address 4.
 *@details  
 *@copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [fengli](li.feng@dfrobot.com)
 *@version  V1.0
 *@date  2023-12-26
 *@https://github.com/DFRobot/DFRobot_LWNode
*/

#include <DFRobot_LWNode.h>

#define REGION_EU868
//#define REGION_US915
//#define REGION_CN470

#ifdef REGION_EU868
	#define FREQ  868100000
#elif defined(REGION_US915)
	#define FREQ    914900000
#elif defined(REGION_CN470)
	#define FREQ    470300000
#endif

DFRobot_LWNode_UART node(3);

void setup( void ){
    Serial.begin(115200);
    #ifdef ESP32
    Serial1.begin(9600, SERIAL_8N1, /*rx =*/D6, /*tx =*/D7);
    #else
    Serial1.begin(9600);
    #endif
    delay(5000);
    node.begin(/*communication UART*/&Serial1,/*debug UART*/&Serial);

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
    node.sendPacket(4, "hello");
    node.sleep(5000);
    
    uint8_t buf[3]={1,2,3};
    node.sendPacket(4, buf, 3);
    node.sleep(5000);
}