#ifndef _SSD1306_H_
#define _SSD1306_H_

/* For uint(X)_t */
#include <stdint.h>

/* For booooool */
#include <stdbool.h>

#include "sdkconfig.h"
#include "ssd1306_err.h"

#define SSD_ALWAYS_INLINE __attribute__( ( always_inline ) )

#define SSD1306_Max_Col 127
#define SSD1306_Max_Row 7

#if ! defined BIT
#define BIT( n ) ( 1 << n )
#endif

typedef enum {
    SSDCmd_Set_Contrast = 0x81,
    SSDCmd_Set_Display_Show_RAM = 0xA4,
    SSDCmd_Set_Display_Ignore_RAM = 0xA5,
    SSDCmd_Set_Normal_Display = 0xA6,
    SSDCmd_Set_Inverted_Display = 0xA7,
    SSDCmd_Set_Display_Off = 0xAE,
    SSDCmd_Set_Display_On = 0xAF,
    SSDCmd_Set_Memory_Addressing_Mode = 0x20,
    SSDCmd_Set_Mux_Ratio = 0xA8,
    SSDCmd_Nop = 0xE3,
    SSDCmd_Set_Display_Offset = 0xD3,
    SSDCmd_Set_Display_Start_Line = 0x40,
    SSDCmd_Set_Display_HFlip_Off = 0xA0,
    SSDCmd_Set_Display_HFlip_On = 0xA1,
    SSDCmd_Set_Display_VFlip_Off = 0xC0,
    SSDCmd_Set_Display_VFlip_On = 0xC8,
    SSDCmd_Set_COM_Pin_Config = 0xDA,
    SSDCmd_Set_Display_CLK = 0xD5,
    SSDCmd_Enable_Charge_Pump_Regulator = 0x8D,
    SSDCmd_Set_Column_Address = 0x21,
    SSDCmd_Set_Page_Address = 0x22
} SSDCmd;

typedef enum {
    AddressMode_Horizontal = 0,
    AddressMode_Vertical,
    AddressMode_Page,
    AddressMode_Invalid
} SSD1306_AddressMode;

struct SSD1306_Device;

/*
 * These can optionally return a succeed/fail but are as of yet unused in the driver.
 */
typedef bool ( *WriteCommandProc ) ( struct SSD1306_Device* DeviceHandle, SSDCmd Command );
typedef bool ( *WriteDataProc ) ( struct SSD1306_Device* DeviceHandle, const uint8_t* Data, size_t DataLength );
typedef bool ( *ResetProc ) ( struct SSD1306_Device* DeviceHandle );

struct spi_device_t;
typedef struct spi_device_t* spi_device_handle_t;

struct SSD1306_FontDef;

struct SSD1306_Device {
    /* I2C Specific */
    int Address;

    /* SPI Specific */
    spi_device_handle_t SPIHandle;
    int RSTPin;
    int CSPin;

    /* Everything else */
    int Width;
    int Height;

    uint8_t* Framebuffer;
    int FramebufferSize;

    WriteCommandProc WriteCommand;
    WriteDataProc WriteData;
    ResetProc Reset;

    const struct SSD1306_FontDef* Font;
    bool FontForceProportional;
    bool FontForceMonospace;
};

void SSD1306_SetMuxRatio( struct SSD1306_Device* DeviceHandle, uint8_t Ratio );
void SSD1306_SetDisplayOffset( struct SSD1306_Device* DeviceHandle, uint8_t Offset );
void SSD1306_SetDisplayStartLines( struct SSD1306_Device* DeviceHandle );
void SSD1306_SetSegmentRemap( struct SSD1306_Device* DeviceHandle, bool Remap );
void SSD1306_SetContrast( struct SSD1306_Device* DeviceHandle, uint8_t Contrast );
void SSD1306_EnableDisplayRAM( struct SSD1306_Device* DeviceHandle );
void SSD1306_DisableDisplayRAM( struct SSD1306_Device* DeviceHandle );
void SSD1306_SetInverted( struct SSD1306_Device* DeviceHandle, bool Inverted );
void SSD1306_SetHFlip( struct SSD1306_Device* DeviceHandle, bool On );
void SSD1306_SetVFlip( struct SSD1306_Device* DeviceHandle, bool On );
void SSD1306_DisplayOn( struct SSD1306_Device* DeviceHandle );
void SSD1306_DisplayOff( struct SSD1306_Device* DeviceHandle ); 
void SSD1306_SetDisplayAddressMode( struct SSD1306_Device* DeviceHandle, SSD1306_AddressMode AddressMode );
void SSD1306_Update( struct SSD1306_Device* DeviceHandle );
void SSD1306_SetDisplayClocks( struct SSD1306_Device* DeviceHandle, uint32_t DisplayClockDivider, uint32_t OSCFrequency );
void SSD1306_WriteRawData( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength );

void SSD1306_SetColumnAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End );
void SSD1306_SetPageAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End );

bool SSD1306_HWReset( struct SSD1306_Device* DeviceHandle );

bool SSD1306_Init_I2C( struct SSD1306_Device* DeviceHandle, int Width, int Height, int I2CAddress, int ResetPin, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset );
bool SSD1306_Init_SPI( struct SSD1306_Device* DeviceHandle, int Width, int Height, int ResetPin, int CSPin, spi_device_handle_t SPIHandle, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset );

#endif
