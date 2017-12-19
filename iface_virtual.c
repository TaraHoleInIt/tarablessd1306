/**
 * Copyright (c) 2017 Tara Keeling
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ssd1306.h"

#ifndef CONFIG_SSD1306_DYNAMIC_ALLOC
#error "Virtual interface requires CONFIG_SSD1306_DYNAMIC_ALLOC"
#endif

static int Virt_WriteCommandStub( struct SSD1306_Device* DeviceHandle, SSDCmd Command ) {
    return 1;
}

static int Virt_WritedataStub( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength ) {
    return 1;
}

int Virt_DeviceInit( struct SSD1306_Device* DeviceHandle, int Width, int Height ) {
    NullCheck( DeviceHandle, return 0 );
    memset( DeviceHandle, 0, sizeof( struct SSD1306_Device ) );

    DeviceHandle->Width = Width;
    DeviceHandle->Height = Height;
    DeviceHandle->FramebufferSize = ( Width * Height ) / 8;
    
    if ( ( DeviceHandle->Framebuffer = ( uint8_t* ) malloc( DeviceHandle->FramebufferSize ) ) != NULL ) {
        memset( DeviceHandle->Framebuffer, 0, DeviceHandle->FramebufferSize );

        DeviceHandle->WriteCommand = Virt_WriteCommandStub;
        DeviceHandle->WriteData = Virt_WritedataStub;

        /* No actual SSD1306 device, so no init needed */
        return 1;
    }

    return 0;
}

void Virt_DeviceBlit( struct SSD1306_Device* SrcDevice, struct SSD1306_Device* DstDevice, struct Rect SrcRect, struct Rect DstRect ) {
    int CopyWidth = 0;
    int CopyHeight = 0;
    int y = 0;
    
    NullCheck( SrcDevice, return );
    NullCheck( SrcDevice->Framebuffer, return );

    NullCheck( DstDevice, return );
    NullCheck( DstDevice->Framebuffer, return );
    
    CopyWidth = ( DstRect.Right - DstRect.Left );
    CopyHeight = ( DstRect.Bottom - DstRect.Top );

    CheckBounds( CopyWidth >= DstDevice->Width, return );
    CheckBounds( CopyHeight >= DstDevice->Height, return );

    CheckBounds( ( DstRect.Left < 0 ) || ( DstRect.Left >= DstDevice->Width ), return );
    CheckBounds( ( DstRect.Right < 0 ) || ( DstRect.Right >= DstDevice->Width ), return );
    CheckBounds( ( DstRect.Top < 0 ) || ( DstRect.Top >= DstDevice->Height ), return );
    CheckBounds( ( DstRect.Bottom < 0 ) || ( DstRect.Bottom >= DstDevice->Height ), return );
    CheckBounds( DstRect.Left >= DstRect.Right, return );
    CheckBounds( DstRect.Top >= DstRect.Bottom, return );

    CheckBounds( ( SrcRect.Left < 0 ) || ( SrcRect.Left >= SrcDevice->Width ), return );
    CheckBounds( ( SrcRect.Right < 0 ) || ( SrcRect.Right >= SrcDevice->Width ), return );
    CheckBounds( ( SrcRect.Top < 0 ) || ( SrcRect.Top >= SrcDevice->Height ), return );
    CheckBounds( ( SrcRect.Bottom < 0 ) || ( SrcRect.Bottom >= SrcDevice->Height ), return );
    CheckBounds( SrcRect.Left >= SrcRect.Right, return );
    CheckBounds( SrcRect.Top >= SrcRect.Bottom, return );

    for ( y = DstRect.Top; y < DstRect.Bottom; y+= 8 ) {
        memcpy( DstDevice->Framebuffer + ( DstDevice->Width * ( y / 8 ) ) + DstRect.Left,
            SrcDevice->Framebuffer + ( SrcDevice->Width * ( y / 8 ) ) + SrcRect.Left,
            CopyWidth
        );
    }
}
