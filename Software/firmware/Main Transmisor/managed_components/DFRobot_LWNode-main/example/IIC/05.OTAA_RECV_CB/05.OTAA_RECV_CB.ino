/*!
 *@file OTAA_RECV_CB.ino
 *@brief The LoRaWAN OTAA mode node, after joining the network, receives data from the gateway using a callback function.
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

void LWRxCB(void *buf, uint16_t size, int8_t rssi, int8_t snr){
    uint8_t *data = (uint8_t *)buf;
    Serial.print("\nsize = ");Serial.println(size);
    Serial.println("HEX:");
    for(uint8_t i=0;i<size;i++){
        Serial.print(data[i],HEX);
    }
    Serial.println();
    Serial.println("Text:");  
    Serial.println((char *)buf);

    Serial.print("rssi=");Serial.println(rssi);
    Serial.print("snr=");Serial.println(snr);
}

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

    node.setRxCB(LWRxCB);

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
  node.sleep(10 * 1000);
}