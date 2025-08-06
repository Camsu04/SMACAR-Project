/*!
 *@file OTAA_RECV_POLL.ino
 *@brief The LoRaWAN OTAA mode node, after joining the network, receives data from the gateway using a polling method.
 *@details  
 *@copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [fengli](li.feng@dfrobot.com)
 *@version  V1.0
 *@date  2023-12-26
 *@https://github.com/DFRobot/DFRobot_LWNode
*/
#include "DFRobot_LWNode.h"

#define REGION_EU868
//#define REGION_US915
//#define REGION_CN470

#ifdef REGION_EU868
	#define DATARATE  DR5
	#define REGION EU868
#elif defined(REGION_US915)
	#define REGION    US915
	#define DATARATE  DR3
	#define SUBBAND   2
#elif defined(REGION_CN470)
	#define REGION    CN470
	#define DATARATE  DR5
	#define SUBBAND   11
#endif

const char _APPEUI[]={"DFDFDFDF00000000"} ;
const char _APPKEY[]={"0102030405060708090A0B0C0D0E0F10"};
uint8_t buf[256]={0x0};

DFRobot_LWNode_IIC node(_APPEUI,_APPKEY);

void setup(void){

    Serial.begin(115200);

    node.begin(/*communication IIC*/&Wire,/*debug UART*/&Serial);
    while(!node.setRegion(REGION)){
        delay(2000);
        Serial.println("REGION set fail");
    }
    while(!node.setDevType(CLASS_C)){
        delay(2000);
        Serial.println("DevType set fail");
    }
    #ifdef SUBBAND
    while(!node.setSubBand(SUBBAND)){
        delay(2000);
        Serial.println("SubBand set fail");
    }
    #endif

    String deveui = node.getDevEUI();
    Serial.print("DEVEUI: ");
    Serial.println(deveui);

    //入网
    if(node.join()){
        Serial.println("JOIN......");
    }

    while(!node.isJoined()){
        delay(5000);
    }
    Serial.println("join success");
}


void loop(){
  //读取缓冲区是否接到数据
  uint8_t len = node.readData(buf);
  
  if(len > 0){
    Serial.print("\nreceive ");Serial.print(len);Serial.println(" bytes  \nHEX:");  
    for(uint8_t i = 0;i<len;i++){
       Serial.print(buf[i],HEX);  
    }
    Serial.println();
    Serial.println("Text:");  
    Serial.println((char *)buf);
  }
  //String data = node.readData();
  //if(data != ""){
  // Serial.println(data);
  //}

  delay(500);
}