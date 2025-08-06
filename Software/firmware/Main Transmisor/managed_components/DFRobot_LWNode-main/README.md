# DFRobot_LWNode
* [中文版](./README_CN.md)<br>
LoRaWan  arduino node

![Product Image](./resources/images/SEN0486.png)

## 产品链接(https://www.dfrobot.com)
    SKU：DFR1115
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary


Provides an Arduino library to control the DFR1115 for LoRa communication.

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods
```C++
  /**
   * @fn setRegion
   * @brief Sets the LoRaWAN region.
   * @param region Region enum value
   * @return Returns true if successful, otherwise false
   */
  bool setRegion(eRegion_t region);

  /**
   * @fn setFreq
   * @brief Sets the frequency.
   * @param freq Frequency value
   * @return Returns true if successful, otherwise false
   */
  bool setFreq(uint32_t freq);

  /**
   * @fn setBW
   * @brief Sets the bandwidth.
   * @param bw Bandwidth value
   * @return Returns true if successful, otherwise false
   */
  bool setBW(uint32_t bw);

  /**
   * @fn setSF
   * @brief Sets the spreading factor.
   * @param sf Spreading factor value
   * @return Returns true if successful, otherwise false
   */
  bool setSF(uint8_t sf);

  /**
   * @fn setRxCB
   * @brief Sets the receive callback function. This function is called when the gateway sends data to the node.
   * @param callback Pointer to the callback function
   */
  void setRxCB(rxCB *callback);

  /**
   * @fn setRxCB
   * @brief Sets the receive callback function for a specific case.
   * @param callback Pointer to the callback function
   */
  void setRxCB(rxCB3 *callback);

  /**
   * @fn setAppEUI
   * @brief Sets the Application EUI.
   * @param appeui Application EUI
   * @return Returns true if successful, otherwise false
   */
  bool setAppEUI(const char *appeui);

  /**
   * @fn setAppKEY
   * @brief Sets the Application Key.
   * @param appkey Application Key
   * @return Returns true if successful, otherwise false
   */
  bool setAppKEY(const char *appkey);

  /**
   * @fn setDevType
   * @brief Sets the device type.
   * @param classType Device class enum value
   * @return Returns true if successful, otherwise false
   */
  bool setDevType(eDeviceClass_t classType);

  /**
   * @fn setDataRate
   * @brief Sets the data rate.
   * @param dataRate Data rate enum value
   * @return Returns true if successful, otherwise false
   */
  bool setDataRate(eDataRate_t dataRate);

  /**
   * @fn setEIRP
   * @brief Sets the transmission power.
   * @param EIRP Transmission power value
   * @return Returns true if successful, otherwise false
   */
  bool setEIRP(uint8_t EIRP);

  /**
   * @fn setSubBand
   * @brief Sets the sub-band.
   * @param subBand Sub-band value
   * @return Returns true if successful, otherwise false
   */
  bool setSubBand(uint8_t subBand);

  /**
   * @fn enableADR
   * @brief Enables or disables Adaptive Data Rate (ADR).
   * @param adr If true, enables ADR; if false, disables ADR
   * @return Returns true if successful, otherwise false
   */
  bool enableADR(bool adr);

  /**
   * @fn setDevAddr
   * @brief Sets the device address.
   * @param devAddr Device address
   * @return Returns true if successful, otherwise false
   */
  bool setDevAddr(const uint32_t devAddr);

  /**
   * @fn setAppSKey
   * @brief Sets the Application Session Key.
   * @param appSKey Application Session Key
   * @return Returns true if successful, otherwise false
   */
  bool setAppSKey(const char *appSKey);

  /**
   * @fn setNwkSKey
   * @brief Sets the Network Session Key.
   * @param nwkSKey Network Session Key
   * @return Returns true if successful, otherwise false
   */
  bool setNwkSKey(const char *nwkSKey);

  /**
   * @fn join
   * @brief Initiates the LoRaWAN join procedure. The device automatically attempts to join the network.
   * @return Returns true if successfully initiated, otherwise false
   */
  bool join();

  /**
   * @fn start
   * @brief Starts the device's operation.
   * @return Returns true if successful, otherwise false
   */
  bool start();

  /**
   * @fn setLoRaAddr
   * @brief Sets the LoRa address.
   * @param addr LoRa address
   * @return Returns true if successful, otherwise false
   */
  bool setLoRaAddr(uint8_t addr);

  /**
   * @fn isJoined
   * @brief Checks if the device is already joined to the network.
   * @return Returns true if joined, otherwise false
   */
  bool isJoined();

  /**
   * @fn sendPacket
   * @brief Sends a data packet.
   * @param v Value to be sent
   * @return Returns true if successful, otherwise false
   */
  bool sendPacket(double v);
  bool sendPacket(int32_t v);
  bool sendPacket(uint32_t v);
  bool sendPacket(void *buffer, uint8_t size);

  /**
   * @fn sendPacket
   * @brief Sends a data packet to a specific address.
   * @param addr Destination address
   * @param v Value to be sent
   * @return Returns true if successful, otherwise false
   */
  bool sendPacket(uint8_t addr, double v);
  bool sendPacket(uint8_t addr, int32_t v);
  bool sendPacket(uint8_t addr, uint32_t v);
  bool sendPacket(uint8_t addr, void *buffer, uint8_t size);

  /**
   * @fn sendPacket
   * @brief Sends a string data packet.
   * @param data String data to be sent
   * @return Returns true if successful, otherwise false
   */
  bool sendPacket(String data);
  
  /**
   * @fn sendPacket
   * @brief Sends a string data packet to a specific address.
   * @param addr Destination address
   * @param data String data to be sent
   * @return Returns true if successful, otherwise false
   */
  bool sendPacket(uint8_t addr, String data);

  /**
   * @fn sendATCmd
   * @brief Sends a generic AT command.
   * @param cmd Preformatted AT command without \r\n
   * @return The response to the AT command
   */
  String sendATCmd(String cmd);

  /**
   * @fn sendATCmdTest
   * @brief Sends a test AT command.
   * @param cmd Test AT command
   * @return The response to the test AT command
   */
  String sendATCmdTest(char *cmd);

  /**
   * @fn setPacketType
   * @brief Sets the packet type.
   * @param type Packet type (CONFIRMED_PACKET or UNCONFIRMED_PACKET)
   * @return Returns true if successful, otherwise false
   */
  bool setPacketType(ePacketType_t type = UNCONFIRMED_PACKET);

  /**
   * @fn getDevEUI
   * @brief Retrieves the device EUI.
   * @return The device EUI as a string
   */
  String getDevEUI();

  /**
   * @fn getNetID
   * @brief Retrieves the network ID.
   * @return 3-byte network ID information
   */
  uint32_t getNetID();

  /**
   * @fn getDevAddr
   * @brief Retrieves the device address. In OTAA mode, this address is assigned by the gateway.
   * @return 4-byte device address information
   */
  uint32_t getDevAddr();

  /**
   * @fn getDataRate
   * @brief Retrieves the current data rate.
   * @return The current data rate
   */
  uint8_t getDataRate();

  /**
   * @fn getEIRP
   * @brief Retrieves the current transmission power.
   * @return The current transmission power
   */
  uint8_t getEIRP();

  /**
   * @fn getRSSI
   * @brief Retrieves the Received Signal Strength Indicator (RSSI).
   * @return The RSSI value
   */
  int16_t getRSSI();

  /**
   * @fn getSNR
   * @brief Retrieves the Signal-to-Noise Ratio (SNR).
   * @return The SNR value
   */
  int8_t getSNR();

  /**
   * @fn atTest
   * @brief Executes an AT test command.
   * @return The result of the test command
   */
  bool atTest();
```

## Compatibility

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Leonardo|      √       |              |             | 
Arduino uno|      √       |              |             | 
Arduino MEGA2560|      √       |              |             | 
FireBeetle-ESP32|      √       |              |             | 
Microbit|      √       |              |             | 


## History

- 2024/07/02 - Version 1.0.0 released.

## Credits

Written by fengli(li.feng@dfrobot.com), 2024.7.2 (Welcome to our [website](https://www.dfrobot.com/))





