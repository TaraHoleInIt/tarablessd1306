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
#include <esp_attr.h>

#include "ssd1306.h"
#include "ssd1306_draw.h"

void IRAM_ATTR SSD1306_DrawPixel( struct SSD1306_Device* DeviceHandle, int X, int Y, int Color ) {
    uint32_t YBit = ( Y & 0x07 );
    uint8_t* FBOffset = NULL;

    /* 
     * We only need to modify the Y coordinate since the pitch
     * of the screen is the same as the width.
     * Dividing Y by 8 gives us which row the pixel is in but not
     * the bit position.
     */
    Y>>= 3;

    FBOffset = DeviceHandle->Framebuffer + ( ( Y * DeviceHandle->Width ) + X );
    *FBOffset = ( Color == SSD_COLOR_WHITE ) ? *FBOffset | BIT( YBit ) : *FBOffset & ~BIT( YBit );
}

void IRAM_ATTR SSD1306_DrawHLine( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
}

void IRAM_ATTR SSD1306_DrawVLine( struct SSD1306_Device* DeviceHandle, int x, int y, int y2, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
}

void IRAM_ATTR SSD1306_DrawLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
}

void IRAM_ATTR SSD1306_DrawRect( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, int y2, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
}

void SSD1306_Clear( struct SSD1306_Device* DeviceHandle, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    memset( DeviceHandle->Framebuffer, Color, DeviceHandle->FramebufferSize );
}
