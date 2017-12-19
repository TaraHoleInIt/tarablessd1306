/**
 * Copyright (c) 2017 Tara Keeling
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

#include "ssd1306.h"

void SSD1306_DrawPixel( struct SSD1306_Device* DeviceHandle, uint32_t X, uint32_t Y, bool Color ) {
    uint32_t YBit = ( Y & 0x07 );
    uint8_t* FBOffset = NULL;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    /* 
     * We only need to modify the Y coordinate since the pitch
     * of the screen is the same as the width.
     * Dividing Y by 8 gives us which row the pixel is in but not
     * the bit position.
     */
    Y>>= 3;

    FBOffset = DeviceHandle->Framebuffer + ( ( Y * DeviceHandle->Width ) + X );
    *FBOffset = ( Color == true ) ? *FBOffset | BIT( YBit ) : *FBOffset & ~BIT( YBit );
}

void SSD1306_DrawHLine( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, bool Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    CheckBounds( x >= DeviceHandle->Width, return );
    CheckBounds( ( x2 + x ) >= DeviceHandle->Width, return );
    CheckBounds( y >= DeviceHandle->Height, return );

    for ( ; x <= x2; x++ ) {
        SSD1306_DrawPixel( DeviceHandle, x, y, Color );
    }
}

void SSD1306_DrawVLine( struct SSD1306_Device* DeviceHandle, int x, int y, int y2, bool Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    CheckBounds( x >= DeviceHandle->Width, return );
    CheckBounds( y >= DeviceHandle->Height, return );
    CheckBounds( ( y2 + y ) >= DeviceHandle->Height, return );

    for ( ; y <= y2; y++ ) {
        SSD1306_DrawPixel( DeviceHandle, x, y, Color );
    }
}

void SSD1306_DrawLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, bool Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    if ( x0 == x1 ) {
        /* Vertical line */
        SSD1306_DrawVLine( DeviceHandle, x0, y0, y1, Color );
    } else if ( y0 == y1 ) {
        /* Horizontal line */
        SSD1306_DrawHLine( DeviceHandle, x0, y0, x1, Color );
    } else {
        /* Diagonal line */
        /* TODO: This */

        CheckBounds( ( x0 < 0 ) || ( x0 >= DeviceHandle->Width ), return );
        CheckBounds( ( x1 < 0 ) || ( x1 >= DeviceHandle->Width ), return );

        CheckBounds( ( y0 < 0 ) || ( y0 >= DeviceHandle->Height ), return );
        CheckBounds( ( y1 < 0 ) || ( y1 >= DeviceHandle->Height ), return );
    }
}

void SSD1306_DrawRect( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, int y2, bool Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    CheckBounds( x >= DeviceHandle->Width, return );
    CheckBounds( ( x2 + x ) >= DeviceHandle->Width, return );
    CheckBounds( y >= DeviceHandle->Height, return );
    CheckBounds( ( y2 + y ) >= DeviceHandle->Height, return );

    for ( ; y <= y2; y++ ) {
        SSD1306_DrawHLine( DeviceHandle, x, y, x2, Color );
    }
}

void SSD1306_Clear( struct SSD1306_Device* DeviceHandle, bool Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    memset( DeviceHandle->Framebuffer, Color, DeviceHandle->FramebufferSize );
}
