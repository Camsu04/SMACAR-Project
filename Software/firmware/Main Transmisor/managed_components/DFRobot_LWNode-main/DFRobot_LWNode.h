/*!
 *@file DFRobot_LWNode.h
 *@brief Define the basic structure of class DFRobot_LWNode, the implementation of basic methods.
 *@details  
 *@copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@license     The MIT license (MIT)
 *@author [fengli](li.feng@dfrobot.com)
 *@version  V1.0
 *@date  2023-12-26
 *@https://github.com/DFRobot/DFRobot_LWNode
*/



#ifndef _DFROBOT_LORANWANNODE_H
#define _DFROBOT_LORANWANNODE_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>
#include <stdint.h>
// #define ENABLE_DBG
#ifdef ENABLE_DBG
#define LDBG(...)  {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define LDBG(...)
#endif

typedef void joinCallback(bool isOk, int16_t rssi, int8_t snr);
typedef void rxCB(void *buffer, uint16_t size, int8_t rssi, int8_t snr);
typedef void rxCB3(uint8_t from, void *buffer, uint16_t size, int8_t rssi, int8_t snr);
typedef void buttonCallback(void);
typedef void sendCallback(void);
#define REG_WRITE_AT_LONG       0x39
#define REG_WRITE_AT            0x40
#define REG_READ_AT_LEN         0x41
#define REG_READ_AT             0x42
#define REG_READ_AT_LONG        0x43
#define REG_READ_DATA           0x45

//该寄存器保存着，数据队列的个数，当主机发送读取这个寄存器时，不管上一个队列有没有读取完
//都将切换到下一个数据
#define REG_READ_NUM_QUEUE			0x46
//目前队列剩余长度
#define REG_READ_DATA_LEN 			0x47

#define REG_READ_NEXT_DATA 			0x48
/**
 * @brief Enumeration for device classes in LoRaWAN.
 */
typedef enum {
  CLASS_A, ///< Class A device
  CLASS_C  ///< Class C device
} eDeviceClass_t;

/**
 * @brief Enumeration for packet types in LoRaWAN.
 */
typedef enum {
  CONFIRMED_PACKET,   ///< Confirmed packet
  UNCONFIRMED_PACKET  ///< Unconfirmed packet
} ePacketType_t;

/**
 * @brief Enumeration for transmit power levels.
 */
typedef enum {
  DBM0 = 0,   ///< 0 dBm
  DBM2 = 2,   ///< 2 dBm
  DBM4 = 4,   ///< 4 dBm
  DBM6 = 6,   ///< 6 dBm
  DBM8 = 8,   ///< 8 dBm
  DBM10 = 10, ///< 10 dBm
  DBM12 = 12, ///< 12 dBm
  DBM14 = 14, ///< 14 dBm
  DBM16 = 16, ///< 16 dBm
  DBM18 = 18, ///< 18 dBm
  DBM20 = 20, ///< 20 dBm
  DBM22 = 22, ///< 22 dBm
  DBM24 = 24, ///< 24 dBm
  DBM26 = 26, ///< 26 dBm
  DBM28 = 28  ///< 28 dBm
} etxPower_t;

/**
 * @brief Enumeration for LoRaWAN regions.
 */
typedef enum {
  EU868, ///< Europe 868 MHz
  US915, ///< USA 915 MHz
  CN470  ///< China 470 MHz
} eRegion_t;

/**
 * @brief Enumeration for data rates.
 */
typedef enum {
  DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7, DR8, DR9, DR10, DR11, DR12
} eDataRate_t;



/**
 * @brief Represents a LoRaWAN node class.
 */
class LWNode {
public:
  /**
   * @fn LWNode
   * @brief Constructor for OTAA devices.
   * @param appEui Application EUI
   * @param appKey Application Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  LWNode(const uint8_t *appEui = nullptr, const uint8_t *appKey = nullptr,
         eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate = DR5,
         etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn LWNode
   * @brief Constructor for ABP devices.
   * @param devAddr Device Address
   * @param nwkSKey Network Session Key
   * @param appSKey Application Session Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  LWNode(const uint32_t devAddr, const uint8_t *nwkSKey, const uint8_t *appSKey,
         eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate = DR5,
         etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn LWNode
   * @brief Constructor for device using only device address.
   * @param devAddr Device Address
   */
  LWNode(const uint8_t devAddr);

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

  /**
   * @fn sendData
   * @brief Pure virtual function for sending data.
   * @param data Pointer to the data to be sent
   * @param len Length of the data
   */
  virtual void sendData(uint8_t *data, uint8_t len) = 0;

  /**
   * @fn readACK
   * @brief Pure virtual function for reading acknowledgment messages.
   * @return The acknowledgment message as a string
   */
  virtual String readACK() = 0;

  char _appeui[17], _appKey[33];
  char _appeSKey[33], _nwkSKey[33];
  bool _isOtaa = true;
  eDeviceClass_t _deviceClass;
  eDataRate_t _dataRate = DR4;
  eDeviceClass_t _classType = CLASS_A;
  etxPower_t _txPower;
  bool _adr = false;
  uint8_t _subBand = 0;
  uint8_t _region = EU868;
  uint32_t _devAddr;
  uint8_t joinType = 1; // 0 ABP, 1 OTAA, 2 NONE

public:
  uint8_t _from;
};




/**
 * @brief LoRaWAN node class for communication via UART.
 */
class DFRobot_LWNode_UART : public LWNode {

public:
  /**
   * @fn DFRobot_LWNode_UART
   * @brief Constructor for OTAA devices.
   * @param appEui Application EUI
   * @param appKey Application Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  DFRobot_LWNode_UART(const char *appEui = nullptr, const char *appKey = nullptr,
                      eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate = DR5,
                      etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn DFRobot_LWNode_UART
   * @brief Constructor for ABP devices.
   * @param devAddr Device Address
   * @param nwkSKey Network Session Key
   * @param appSKey Application Session Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  DFRobot_LWNode_UART(const uint32_t devAddr, const char *nwkSKey, const char *appSKey,
                      eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate = DR5,
                      etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn DFRobot_LWNode_UART
   * @brief Constructor with address for device communication.
   * @param from Communication address
   */
  DFRobot_LWNode_UART(const uint8_t from);

  /**
   * @fn begin
   * @brief Initializes the LoRaWAN node.
   * @param s_ UART object for communication
   * @param dbgs_ Serial object for debugging (default: Serial)
   * @return true if initialization is successful, false otherwise
   */
  bool begin(Stream *s_, Stream *dbgs_ = &Serial);

  /**
   * @fn sleep
   * @brief Puts the device to sleep for a specified duration.
   * @param ms Duration in milliseconds
   */
  void sleep(uint32_t ms);

  /**
   * @fn readData
   * @brief Reads data from the device.
   * @return Data message as a string
   */
  String readData();

  /**
   * @fn readData
   * @brief Reads data into a buffer.
   * @param buf Buffer to store the read data
   * @return Number of bytes read
   */
  size_t readData(uint8_t *buf);

  /**
   * @fn sendData
   * @brief Sends data to the device.
   * @param data Pointer to the data to be sent
   * @param len Length of the data
   */
  void sendData(uint8_t *data, uint8_t len);

  /**
   * @fn readACK
   * @brief Reads the acknowledgment message.
   * @return Acknowledgment message as a string
   */
  String readACK();

private:
  Stream *s;    ///< UART communication stream
  Stream *dbgs; ///< Debugging information stream
};


/**
 * @brief LoRaWAN node class for communication via IIC.
 */
class DFRobot_LWNode_IIC : public LWNode {

public:
  /**
   * @fn DFRobot_LWNode_IIC
   * @brief Constructor for OTAA devices.
   * @param appEui Application EUI
   * @param appKey Application Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  DFRobot_LWNode_IIC(const char *appEui = nullptr, const char *appKey = nullptr,
                     eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate =DR5,
                     etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn DFRobot_LWNode_IIC
   * @brief Constructor for ABP devices.
   * @param devAddr Device Address
   * @param nwkSKey Network Session Key
   * @param appSKey Application Session Key
   * @param classType Device class (default: CLASS_C)
   * @param dataRate Data rate (default: DR5)
   * @param txPower Transmission power (default: DBM8)
   * @param adr Adaptive Data Rate (default: true)
   * @param subBand Sub-band for CN470 and US915 (default: 11)
   */
  DFRobot_LWNode_IIC(const uint32_t devAddr, const char *nwkSKey, const char *appSKey,
                     eDeviceClass_t classType = CLASS_C, eDataRate_t dataRate = DR5,
                     etxPower_t txPower = DBM8, bool adr = true, uint8_t subBand = 11);

  /**
   * @fn DFRobot_LWNode_IIC
   * @brief Constructor with address for device communication.
   * @param from Communication address
   */
  DFRobot_LWNode_IIC(const uint8_t from);

  /**
   * @fn begin
   * @brief Initializes the LoRaWAN node.
   * @param pWire IIC object for communication (default: Wire)
   * @param dbgs_ Serial object for debugging (default: Serial)
   * @return true if initialization is successful, false otherwise
   */
  bool begin(TwoWire *pWire = &Wire, Stream *dbgs_ = &Serial);

  /**
   * @fn sleep
   * @brief Puts the device to sleep for a specified duration.
   * @param ms Duration in milliseconds
   */
  void sleep(uint32_t ms);

  /**
   * @fn sendData
   * @brief Sends data to the device.
   * @param data Pointer to the data to be sent
   * @param len Length of the data
   */
  void sendData(uint8_t *data, uint8_t len);

  /**
   * @fn readACK
   * @brief Reads the acknowledgment message.
   * @return Acknowledgment message as a string
   */
  String readACK();

  /**
   * @fn readData
   * @brief Reads data from the device.
   * @return Data message as a string
   */
  String readData();

  /**
   * @fn readData
   * @brief Reads data into a buffer.
   * @param buf Buffer to store the read data
   * @return Number of bytes read
   */
  size_t readData(uint8_t *buf);

  /**
   * @fn readReg
   * @brief Reads data from a register.
   * @param reg Register address
   * @param data Buffer to store the read data
   * @param length Length of the data to be read
   * @return 1 if successful, 0 otherwise
   */
  uint8_t readReg(uint16_t reg, uint8_t data[], uint8_t length);

  /**
   * @fn writeReg
   * @brief Writes data to a register.
   * @param reg Register address
   * @param data Pointer to the data to be written
   * @param len Length of the data
   */
  void writeReg(uint8_t reg, uint8_t *data, uint8_t len);

  /**
   * @fn readReg
   * @brief Reads a single register.
   * @param reg Register address
   * @return Value of the register
   */
  int8_t readReg(uint8_t reg);

  /**
   * @fn readLoraData
   * @brief Reads data from LoRaWAN and LoRa.
   * @return Data in string format.
   */
  String readLoraData();

   /**
   * @fn available
   * @brief Reads data from LoRaWAN and LoRa.
   * @return Data in string format.
   */
  uint8_t available();


     /**
   * @fn available
   * @brief Reads data from LoRaWAN and LoRa.
   * @return Data in string format.
   */
  int8_t readSNR();

       /**
   * @fn available
   * @brief Reads data from LoRaWAN and LoRa.
   * @return Data in string format.
   */
  int8_t readRSSI();

       /**
   * @fn available
   * @brief Reads data from LoRaWAN and LoRa.
   * @return Data in string format.
   */
  uint8_t readAddrFROM();

private:
  TwoWire *_pWire; ///< IIC communication object
  uint8_t _deviceAddr; ///< IIC device address
  Stream *dbgs; ///< Debugging information stream

  int8_t SNR;
  int8_t RSSI;
  uint8_t FROM;
  
};



#endif
