/**
 * Copyright (c) 2017-2018 Tara Keeling
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <esp_heap_caps.h>

#include "ssd1306.h"

#define COM_Disable_LR_Remap 0
#define COM_Enable_LR_Remap BIT( 5 )

#define COM_Pins_Sequential 0
#define COM_Pins_Alternative BIT( 4 )

#define COM_ScanDir_LR 0
#define COM_ScanDir_RL 1

static bool SSD1306_Init( struct SSD1306_Device* DeviceHandle, int Width, int Height );

bool SSD1306_WriteCommand( struct SSD1306_Device* DeviceHandle, SSDCmd SSDCommand ) {
    NullCheck( DeviceHandle, return false );
    NullCheck( DeviceHandle->WriteCommand, return false );

    return ( DeviceHandle->WriteCommand ) ( DeviceHandle, SSDCommand );
}

bool SSD1306_WriteData( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength ) {
    NullCheck( DeviceHandle, return false );
    NullCheck( DeviceHandle->WriteData, return false );

    return ( DeviceHandle->WriteData ) ( DeviceHandle, Data, DataLength );
}

void SSD1306_SetMuxRatio( struct SSD1306_Device* DeviceHandle, uint8_t Ratio ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, 0xA8 );
    SSD1306_WriteCommand( DeviceHandle, Ratio );
}

void SSD1306_SetDisplayOffset( struct SSD1306_Device* DeviceHandle, uint8_t Offset ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, 0xD3 );
    SSD1306_WriteCommand( DeviceHandle, Offset );
}

void SSD1306_SetDisplayStartLine( struct SSD1306_Device* DeviceHandle, int Line ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, 
        SSDCmd_Set_Display_Start_Line + ( uint32_t ) ( Line & 0x1F )
    );
}

/*
 * This is all a big giant mystery that I have yet to figure out.
 * Beware all ye who enter.
 */
static void SetCOMPinConfiguration( struct SSD1306_Device* DeviceHandle, uint32_t RemapCFG, uint32_t PinCFG, int ScanDir ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_COM_Pin_Config );
    SSD1306_WriteCommand( DeviceHandle, ( uint8_t ) ( RemapCFG | PinCFG | BIT( 1 ) ) );

    SSD1306_WriteCommand( DeviceHandle, 
        ( ScanDir == COM_ScanDir_LR ) ? SSDCmd_Set_Display_VFlip_Off : SSDCmd_Set_Display_VFlip_On
    );
}

void SSD1306_SetContrast( struct SSD1306_Device* DeviceHandle, uint8_t Contrast ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Contrast );
    SSD1306_WriteCommand( DeviceHandle, Contrast );
}

void SSD1306_EnableDisplayRAM( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Display_Show_RAM );
}

void SSD1306_DisableDisplayRAM( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Display_Ignore_RAM );
}

void SSD1306_SetInverted( struct SSD1306_Device* DeviceHandle, bool Inverted ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, ( Inverted == true ) ? SSDCmd_Set_Inverted_Display : SSDCmd_Set_Normal_Display );
}

void SSD1306_SetDisplayClocks( struct SSD1306_Device* DeviceHandle, uint32_t DisplayClockDivider, uint32_t OSCFrequency ) {
    NullCheck( DeviceHandle, return );

    DisplayClockDivider&= 0x0F;
    OSCFrequency&= 0x0F;

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Display_CLK );
    SSD1306_WriteCommand( DeviceHandle, ( ( OSCFrequency << 4 ) | DisplayClockDivider ) );
}

/* There is no documentation for this command, but it is required during init. */
static void EnableChargePumpRegulator( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Enable_Charge_Pump_Regulator );
    SSD1306_WriteCommand( DeviceHandle, 0x14 ); /* MAGIC NUMBER */
}

void SSD1306_DisplayOn( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Display_On );
}

void SSD1306_DisplayOff( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Display_Off );
}

void SSD1306_SetDisplayAddressMode( struct SSD1306_Device* DeviceHandle, SSD1306_AddressMode AddressMode ) {
    NullCheck( DeviceHandle, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Memory_Addressing_Mode );
    SSD1306_WriteCommand( DeviceHandle, AddressMode );
}

void SSD1306_Update( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteData( DeviceHandle, DeviceHandle->Framebuffer, DeviceHandle->FramebufferSize );
}

void SSD1306_WriteRawData( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength ) {
    NullCheck( DeviceHandle, return );
    NullCheck( Data, return );

    DataLength = DataLength > DeviceHandle->FramebufferSize ? DeviceHandle->FramebufferSize : DataLength;

    if ( DataLength > 0 ) {
        SSD1306_WriteData( DeviceHandle, Data, DataLength );
    }
}

void SSD1306_SetHFlip( struct SSD1306_Device* DeviceHandle, bool On ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, ( On == true ) ? SSDCmd_Set_Display_HFlip_On : SSDCmd_Set_Display_HFlip_Off );
}

void SSD1306_SetVFlip( struct SSD1306_Device* DeviceHandle, bool On ) {
    NullCheck( DeviceHandle, return );
    SSD1306_WriteCommand( DeviceHandle, ( On == true ) ? SSDCmd_Set_Display_VFlip_On : SSDCmd_Set_Display_VFlip_Off );
}

void SSD1306_SetColumnAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End ) {
    NullCheck( DeviceHandle, return );

    CheckBounds( Start > SSD1306_Max_Col, return );
    CheckBounds( End > SSD1306_Max_Col, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Column_Address );
    SSD1306_WriteCommand( DeviceHandle, Start );
    SSD1306_WriteCommand( DeviceHandle, End );
}

void SSD1306_SetPageAddress( struct SSD1306_Device* DeviceHandle, uint8_t Start, uint8_t End ) {
    NullCheck( DeviceHandle, return );

    CheckBounds( Start > SSD1306_Max_Row, return );
    CheckBounds( End > SSD1306_Max_Row, return );

    SSD1306_WriteCommand( DeviceHandle, SSDCmd_Set_Page_Address );
    SSD1306_WriteCommand( DeviceHandle, Start );
    SSD1306_WriteCommand( DeviceHandle, End );
}

bool SSD1306_HWReset( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return 0 );

    if ( DeviceHandle->Reset != NULL ) {
        return ( DeviceHandle->Reset ) ( DeviceHandle );
    }

    /* This should always return true if there is no reset callback as
     * no error would have occurred during the non existant reset.
     */
    return true;
}

static bool SSD1306_Init( struct SSD1306_Device* DeviceHandle, int Width, int Height ) {
    DeviceHandle->Width = Width;
    DeviceHandle->Height = Height;
    DeviceHandle->FramebufferSize = ( DeviceHandle->Width * Height ) / 8;

    DeviceHandle->Framebuffer = heap_caps_calloc( 1, DeviceHandle->FramebufferSize, MALLOC_CAP_DMA | MALLOC_CAP_8BIT );

    NullCheck( DeviceHandle->Framebuffer, return false );

    /* For those who have a hardware reset pin on their display */
    SSD1306_HWReset( DeviceHandle );
    
    /* Init sequence according to SSD1306.pdf */
    SSD1306_SetMuxRatio( DeviceHandle, 0x3F );
    SSD1306_SetDisplayOffset( DeviceHandle, 0x00 );
    SSD1306_SetDisplayStartLine( DeviceHandle, 0 );
    SSD1306_SetHFlip( DeviceHandle, false );
    SSD1306_SetVFlip( DeviceHandle, false );

    if ( Height == 64 ) {
        SetCOMPinConfiguration( DeviceHandle, COM_Disable_LR_Remap, COM_Pins_Alternative, COM_ScanDir_LR );
    } else {
        SetCOMPinConfiguration( DeviceHandle, COM_Disable_LR_Remap, COM_Pins_Sequential, COM_ScanDir_LR );
    }
    
    SSD1306_SetContrast( DeviceHandle, 0x7F );
    SSD1306_DisableDisplayRAM( DeviceHandle );
    SSD1306_SetInverted( DeviceHandle, false );
    SSD1306_SetDisplayClocks( DeviceHandle, 0, 8 );
    EnableChargePumpRegulator( DeviceHandle );
    SSD1306_SetDisplayAddressMode( DeviceHandle, AddressMode_Horizontal );
    SSD1306_SetColumnAddress( DeviceHandle, 0, DeviceHandle->Width - 1 );
    SSD1306_SetPageAddress( DeviceHandle, 0, ( DeviceHandle->Height / 8 ) - 1 );
    SSD1306_EnableDisplayRAM( DeviceHandle );
    SSD1306_DisplayOn( DeviceHandle );
    SSD1306_Update( DeviceHandle );

    return true;
}

bool SSD1306_Init_I2C( struct SSD1306_Device* DeviceHandle, int Width, int Height, int I2CAddress, int ResetPin, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset ) {
    NullCheck( DeviceHandle, return false );
    NullCheck( WriteCommand, return false );
    NullCheck( WriteData, return false );

    memset( DeviceHandle, 0, sizeof( struct SSD1306_Device ) );

    DeviceHandle->WriteCommand = WriteCommand;
    DeviceHandle->WriteData = WriteData;
    DeviceHandle->Reset = Reset;
    DeviceHandle->Address = I2CAddress;
    DeviceHandle->RSTPin = ResetPin;
    
    return SSD1306_Init( DeviceHandle, Width, Height );
}

bool SSD1306_Init_SPI( struct SSD1306_Device* DeviceHandle, int Width, int Height, int ResetPin, int CSPin, spi_device_handle_t SPIHandle, WriteCommandProc WriteCommand, WriteDataProc WriteData, ResetProc Reset ) {
    NullCheck( DeviceHandle, return false );
    NullCheck( WriteCommand, return false );
    NullCheck( WriteData, return false );

    memset( DeviceHandle, 0, sizeof( struct SSD1306_Device ) );

    DeviceHandle->WriteCommand = WriteCommand;
    DeviceHandle->WriteData = WriteData;
    DeviceHandle->Reset = Reset;
    DeviceHandle->SPIHandle = SPIHandle;
    DeviceHandle->RSTPin = ResetPin;
    DeviceHandle->CSPin = CSPin;

    return SSD1306_Init( DeviceHandle, Width, Height );
}
