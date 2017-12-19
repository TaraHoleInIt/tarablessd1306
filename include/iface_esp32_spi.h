#ifndef _IFACE_ESP32_SPI_H_
#define _IFACE_ESP32_SPI_H_

int ESP32_WriteCommand_SPI( struct SSD1306_Device* DeviceHandle, SSDCmd SSDCommand );
int ESP32_WriteData_SPI( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength );
void ESP32_SPI_ResetDisplay( void);
int ESP32_InitSPIMaster( int DC );
int ESP32_AddDevice_SPI( struct SSD1306_Device* DeviceHandle, int Width, int Height, int CSPin, int RSTPin );

#endif
