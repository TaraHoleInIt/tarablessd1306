#ifndef _SSD1306_H_
#define _SSD1306_H_

/* For uint(X)_t */
#include <stdint.h>

/* For booooool */
#include <stdbool.h>

#include "config.h"

#define SSD1306_Max_Framebuffer_Size ( ( 128 * 64 ) / 8 )
#define SSD1306_Max_Col 127
#define SSD1306_Max_Row 7

#ifndef BIT
#define BIT( n ) ( 1 << n )
#endif

#define CheckBounds( expr, retexpr ) { \
    if ( expr ) { \
        printf( "[%s:%d] %s\n", __FUNCTION__, __LINE__, #expr ); \
        retexpr; \
    } \
}

#define NullCheck( ptr, retexpr ) { \
    if ( ptr == NULL ) { \
        printf( "%s: %s == NULL\n", __FUNCTION__, #ptr ); \
        retexpr; \
    }; \
}

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
struct FontDef;

typedef int ( *WriteCommandProc ) ( struct SSD1306_Device* DeviceHandle, SSDCmd Command );
typedef int ( *WriteDataProc ) ( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength );
typedef int ( *ResetProc ) ( struct SSD1306_Device* DeviceHandle );

struct SSD1306_Device {
    /* I2C Specific */
    int Address;

    /* SPI Specific */
    int RSTPin;
    int CSPin;

    /* Everything else */
    int Width;
    int Height;

#ifdef CONFIG_SSD1306_DYNAMIC_ALLOC
    uint8_t* Framebuffer;
#else
    uint8_t Framebuffer[ SSD1306_Max_Framebuffer_Size ];
#endif

    int FramebufferSize;

    struct FontDef* Font;

    WriteCommandProc WriteCommand;
    WriteDataProc WriteData;
    ResetProc Reset;

    /* Can be anything, a good use might be a device handle for I2C or SPI */
    uint32_t User0;
};

struct Rect {
    int Left;
    int Right;
    int Top;
    int Bottom;
};

static inline struct Rect MakeRect( int Left, int Right, int Top, int Bottom ) {
    struct Rect Temp;

    Temp.Left = Left;
    Temp.Right = Right;
    Temp.Top = Top;
    Temp.Bottom = Bottom;

    return Temp;
}

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

void SSD1306_Clear( struct SSD1306_Device* DeviceHandle, bool Color );
void SSD1306_DrawPixel( struct SSD1306_Device* DeviceHandle, uint32_t X, uint32_t Y, bool Color );
void SSD1306_DrawHLine( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, bool Color );
void SSD1306_DrawVLine( struct SSD1306_Device* DeviceHandle, int x, int y, int y2, bool Color );
void SSD1306_DrawLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, bool Color );
void SSD1306_DrawRect( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, int y2, bool Color );
void SSD1306_SetFont( struct SSD1306_Device* DeviceHandle, struct FontDef* FontHandle );

void SSD1306_SetColumnAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End );
void SSD1306_SetPageAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End );

int SSD1306_HWReset( struct SSD1306_Device* DeviceHandle );

int SSD1306_Init_I2C( struct SSD1306_Device* DeviceHandle, int Width, int Height, int I2CAddress, uint32_t UserParam, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset );
int SSD1306_Init_SPI( struct SSD1306_Device* DeviceHandle, int Width, int Height, int ResetPin, int CSPin, uint32_t UserParam, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset );

#endif
