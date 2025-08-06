/*!
 *@file getDEVEUI.ino
 *@brief The LoRaWAN node retrieves the device's DEVEUI (Device EUI).
 *@details  
 *@copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [fengli](li.feng@dfrobot.com)
 *@version  V1.0
 *@date  2023-12-26
 *@https://github.com/DFRobot/DFRobot_LWNode
*/


#include "DFRobot_LWNode.h"
const char _APPEUI[]={"DFDFDFDF00000000"} ;
const char _APPKEY[]={"0102030405060708090A0B0C0D0E0F10"};



DFRobot_LWNode_UART node(_APPEUI,_APPKEY);
void setup(void){
  Serial.begin(115200);
  #ifdef ESP32
  Serial1.begin(9600, SERIAL_8N1, /*rx =*/D6, /*tx =*/D7);
  #else
  Serial1.begin(9600);
  #endif
  node.begin(/*communication uart*/&Serial1,/*debug uart*/&Serial);
  delay(5000);
}


void loop(){
  String deveui = node.getDevEUI();
  Serial.print("DEVEUI: ");
  Serial.println(deveui);
  node.sleep(10 * 1000);
}