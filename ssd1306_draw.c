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

__attribute__( ( always_inline ) ) static inline bool IsPixelVisible( struct SSD1306_Device* DeviceHandle, int x, int y )  {
    bool Result = (
        ( x >= 0 ) &&
        ( x < DeviceHandle->Width ) &&
        ( y >= 0 ) &&
        ( y < DeviceHandle->Height )
    ) ? true : false;

#if CONFIG_SSD1306_CLIPDEBUG > 0
    if ( Result == false ) {
        ClipDebug( x, y );
    }
#endif

    return Result;
}

__attribute__( ( always_inline ) ) static inline void SwapInt( int* a, int* b ) {
    int Temp = *b;

    *b = *a;
    *a = Temp;
}

static inline void IRAM_ATTR SSD1306_DrawPixelFast( struct SSD1306_Device* DeviceHandle, int X, int Y, int Color ) {
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

    if ( Color == SSD_COLOR_XOR ) {
        *FBOffset ^= BIT( YBit );
    } else {
        *FBOffset = ( Color == SSD_COLOR_WHITE ) ? *FBOffset | BIT( YBit ) : *FBOffset & ~BIT( YBit );
    }
}

void IRAM_ATTR SSD1306_DrawPixel( struct SSD1306_Device* DeviceHandle, int x, int y, int Color ) {
    NullCheck( DeviceHandle, return );

    if ( IsPixelVisible( DeviceHandle, x, y ) == true ) {
        SSD1306_DrawPixelFast( DeviceHandle, x, y, Color );
    }
}

void IRAM_ATTR SSD1306_DrawHLine( struct SSD1306_Device* DeviceHandle, int x, int y, int Width, int Color ) {
    int XEnd = x + Width;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    for ( ; x <= XEnd; x++ ) {
        if ( IsPixelVisible( DeviceHandle, x, y ) == true ) {
            SSD1306_DrawPixelFast( DeviceHandle, x, y, Color );
        } else {
            break;
        }
    }
}

void IRAM_ATTR SSD1306_DrawVLine( struct SSD1306_Device* DeviceHandle, int x, int y, int Height, int Color ) {
    int YEnd = y + Height;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    for ( ; y <= YEnd; y++ ) {
        if ( IsPixelVisible( DeviceHandle, x, y ) == true ) {
            SSD1306_DrawPixel( DeviceHandle, x, y, Color );
        } else {
            break;
        }
    }
}

static inline void IRAM_ATTR DrawWideLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, int Color ) {
    int dx = ( x1 - x0 );
    int dy = ( y1 - y0 );
    int Error = 0;
    int Incr = 1;
    int x = x0;
    int y = y0;

    if ( dy < 0 ) {
        Incr = -1;
        dy = -dy;
    }

    Error = ( dy * 2 ) - dx;

    for ( ; x <= x1; x++ ) {
        if ( IsPixelVisible( DeviceHandle, x, y ) == true ) {
            SSD1306_DrawPixelFast( DeviceHandle, x, y, Color );
        }

        if ( Error > 0 ) {
            Error-= ( dx * 2 );
            y+= Incr;
        }

        Error+= ( dy * 2 );
    }
}

static inline void IRAM_ATTR DrawTallLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, int Color ) {
    int dx = ( x1 - x0 );
    int dy = ( y1 - y0 );
    int Error = 0;
    int Incr = 1;
    int x = x0;
    int y = y0;

    if ( dx < 0 ) {
        Incr = -1;
        dx = -dx;
    }

    Error = ( dx * 2 ) - dy;

    for ( ; y < y1; y++ ) {
        if ( IsPixelVisible( DeviceHandle, x, y ) == true ) {
            SSD1306_DrawPixelFast( DeviceHandle, x, y, Color );
        }

        if ( Error > 0 ) {
            Error-= ( dy * 2 );
            x+= Incr;
        }

        Error+= ( dx * 2 );
    }
}

void IRAM_ATTR SSD1306_DrawLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    if ( x0 == x1 ) {
        SSD1306_DrawVLine( DeviceHandle, x0, y0, ( y1 - y0 ), Color );
    } else if ( y0 == y1 ) {
        SSD1306_DrawHLine( DeviceHandle, x0, y0, ( x1 - x0 ), Color );
    } else {
        if ( abs( x1 - x0 ) > abs( y1 - y0 ) ) {
            /* Wide ( run > rise ) */
            if ( x0 > x1 ) {
                SwapInt( &x0, &x1 );
                SwapInt( &y0, &y1 );
            }

            DrawWideLine( DeviceHandle, x0, y0, x1, y1, Color );
        } else {
            /* Tall ( rise > run ) */
            if ( y0 > y1 ) {
                SwapInt( &y0, &y1 );
                SwapInt( &x0, &x1 );
            }

            DrawTallLine( DeviceHandle, x0, y0, x1, y1, Color );
        }
    }
}

void IRAM_ATTR SSD1306_DrawBox( struct SSD1306_Device* DeviceHandle, int x1, int y1, int x2, int y2, int Color, bool Fill ) {
    int Width = ( x2 - x1 );
    int Height = ( y2 - y1 );

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    if ( Fill == false ) {
        /* Top side */
        SSD1306_DrawHLine( DeviceHandle, x1, y1, Width, Color );

        /* Bottom side */
        SSD1306_DrawHLine( DeviceHandle, x1, y1 + Height, Width, Color );

        /* Left side */
        SSD1306_DrawVLine( DeviceHandle, x1, y1, Height, Color );

        /* Right side */
        SSD1306_DrawVLine( DeviceHandle, x1 + Width, y1, Height, Color );
    } else {
        /* Fill the box by drawing horizontal lines */
        for ( ; y1 <= y2; y1++ ) {
            SSD1306_DrawHLine( DeviceHandle, x1, y1, Width, Color );
        }
    }
}

void SSD1306_Clear( struct SSD1306_Device* DeviceHandle, int Color ) {
    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );

    memset( DeviceHandle->Framebuffer, Color, DeviceHandle->FramebufferSize );
}
