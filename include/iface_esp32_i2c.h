#ifndef _IFACE_ESP32_I2C_H_
#define _IFACE_ESP32_I2C_H_

#define USE_THIS_I2C_PORT I2C_NUM_1

int ESP32_WriteCommand_I2C( struct SSD1306_Device* DeviceHandle, SSDCmd SSDCommand );
int ESP32_WriteData_I2C( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength );
int ESP32_InitI2CMaster( int SDA, int SCL );

#endif
