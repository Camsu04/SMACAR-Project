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


void uartRxCB(void *buffer, uint16_t size, int8_t rssi, int8_t snr){
    uint8_t *data = (uint8_t *)buffer;
    Serial.print("\nsize = ");Serial.println(size);
    Serial.println("HEX:");
    for(uint8_t i=0;i<size;i++){
        Serial.print(data[i],HEX);
    }
    Serial.println();
    Serial.println("Text:");
    Serial.println((char *)buffer);

    Serial.print("rssi=");Serial.println(rssi);
    Serial.print("snr=");Serial.println(snr);
}

DFRobot_LWNode_UART node(_APPEUI,_APPKEY);

void setup(void){
    Serial.begin(115200);
    #ifdef ESP32
    Serial1.begin(9600, SERIAL_8N1, /*rx =*/D6, /*tx =*/D7);
    #else
    Serial1.begin(9600);
    #endif
    delay(5000);
    node.begin(/*communication uart*/&Serial1,/*debug uart*/&Serial);
    
    while(!node.setRegion(REGION)){
        delay(2000);
        Serial.println("REGION set fail");
    }
    while(!node.setAppEUI(_APPEUI)){
        delay(2000);
        Serial.println("AppEUI set fail");
    }
    while(!node.setAppKEY(_APPKEY)){
        delay(2000);
        Serial.println("AppKEY set fail");
    }
    while(!node.setDevType(CLASS_C)){
        delay(2000);
        Serial.println("DevType set fail");
    }
    
    //EU868 DR0  - DR5
    //US915 DR0  - DR3
    //CN470 DR0  - DR5
    while (!node.setDataRate(DATARATE)) {
        delay(2000);
        Serial.println("DataRate set fail");
    }
    
    //EU868 DBM0  DBM2 DBM4 DBM6 DBM8 DBM10 DBM12 DBM14 DBM16
    //US915 DBM0  DBM2 DBM4 DBM6 DBM8 DBM10 DBM12 DBM14 DBM16 DBM18 DBM20 DBM22
    //CN470 DBM0  DBM2 DBM4 DBM6 DBM8 DBM10 DBM12 DBM14 DBM16 DBM18 
    while (!node.setEIRP(DBM16)) {
        Serial.println("EIRP set fail");
    }
    while (!node.setPacketType(UNCONFIRMED_PACKET)) {
      Serial.println("Packet type set fail");
    }
    #ifdef SUBBAND
    while(!node.setSubBand(SUBBAND)){
         Serial.println("SubBand set fail");
    }
    #endif

    while(!node.enableADR(false)){
        delay(2000);
        Serial.println("ADR set fail");
    }

    String deveui = node.getDevEUI();
    Serial.print("DEVEUI: ");
    Serial.println(deveui);
    
    node.setRxCB(uartRxCB);
    //join
    if(node.join()){
        Serial.println("JOIN......");
    }
    
    while(!node.isJoined()){
        delay(5000);
    }
    Serial.println("join success");
}


void loop(){
  node.sleep(9000);
}