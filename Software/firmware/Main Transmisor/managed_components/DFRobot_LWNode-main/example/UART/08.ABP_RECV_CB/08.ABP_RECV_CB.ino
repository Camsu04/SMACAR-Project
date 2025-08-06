/*!
 *@file ABP_RECV_CB.ino
 *@brief The LoRaWAN ABP mode node receives data from the gateway using a callback function.
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

const char NWKSKEY[]={"87888888888888888888888888888888"};
const char APPSKEY[]={"89888888888888888888888888888888"};

uint32_t devAddr = 0xDF000011;

void uartRxCB(void *buf, uint16_t size, int8_t rssi, int8_t snr){
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

DFRobot_LWNode_UART node(devAddr,NWKSKEY,APPSKEY);

void setup(void){
    Serial.begin(115200);
    #ifdef ESP32
    Serial1.begin(9600, SERIAL_8N1, /*rx =*/D6, /*tx =*/D7);
    #else
    Serial1.begin(9600);
    #endif
    node.begin(/*communication uart*/&Serial1,/*debug uart*/&Serial);
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
         Serial.println("SubBand set fail");
    }
    #endif
    
    String deveui = node.getDevEUI();
    Serial.print("DEVEUI: ");
    Serial.println(deveui);

    node.setRxCB(uartRxCB);
    node.start();
}


void loop(){
  //读取缓冲区是否接到数据
  node.sleep(10 * 1000);
}