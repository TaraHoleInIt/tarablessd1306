/**
 * Copyright (c) 2017 Tara Keeling
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ssd1306.h"
#include "font.h"

int FontGetCharHeight( struct FontDef* FontHandle, char c ) {
    NullCheck( FontHandle, return 0 );
    NullCheck( FontHandle->Data, return 0 );

    return FontHandle->Height * 8;
}

int FontMeasureString( struct FontDef* FontHandle, const char* Text ) {
    int StringWidthInPixels = 0;
    int Length = 0;
    int i = 0;

    NullCheck( FontHandle, return 0 );
    NullCheck( FontHandle->Data, return 0 );
    NullCheck( Text, return 0 );

    for ( i = 0, Length = strlen( Text ); i < Length; i++ ) {
        StringWidthInPixels+= FontGetCharWidth( FontHandle, Text[ i ] );
    }

    return StringWidthInPixels;
}

int FontGetCharWidth( struct FontDef* FontHandle, char c ) {
    const uint8_t* WidthOffset = FontHandle->Data;

    NullCheck( FontHandle, return 0 );
    NullCheck( FontHandle->Data, return 0 );

    if ( c < FontHandle->StartChar || c > FontHandle->EndChar )
        return 0;

    WidthOffset+= ( c - FontHandle->StartChar ) * ( ( FontHandle->Width * FontHandle->Height ) + 1 );
    return *WidthOffset;
}

void FontDrawChar( struct SSD1306_Device* DeviceHandle, char c, int x, int y, bool Color ) {
    const uint8_t* FontOffset = NULL;
    struct FontDef* Font = NULL;
    int GlyphSizeInBytes = 0;
    int CharHeight = 0;
    int CharWidth = 0;
    int x2 = 0;
    int y2 = 0;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
    NullCheck( DeviceHandle->Font, return );
    NullCheck( DeviceHandle->Font->Data, return );

    CharWidth = FontGetCharWidth( DeviceHandle->Font, c );
    CharHeight = FontGetCharHeight( DeviceHandle->Font, c );

    CheckBounds( x >= ( DeviceHandle->Width ) - CharWidth, return );
    CheckBounds( y >= ( DeviceHandle->Height ) - CharHeight, return );

    if ( c < DeviceHandle->Font->StartChar || c > DeviceHandle->Font->EndChar ) {
        return;
    }

    Font = DeviceHandle->Font;

    /* Divide y by 8 to get which page the Y coordinate is on */
    y>>= 3;

    /* Height is size in bytes, so a 16px high font would be 2 bytes tall */
    GlyphSizeInBytes = ( Font->Width * Font->Height ) + 1;
    FontOffset = &Font->Data[ ( ( c - Font->StartChar ) * GlyphSizeInBytes ) + 1 ];

    for ( x2 = 0; x2 < FontGetCharWidth( Font, c ); x2++ ) {
        for ( y2 = 0; y2 < Font->Height; y2++ ) {
            DeviceHandle->Framebuffer[ ( ( y2 + y ) * DeviceHandle->Width ) + ( x + x2 ) ] = ( Color == true ) ? *FontOffset : ! *FontOffset;
            FontOffset++;
        }
    }
}

void FontDrawString( struct SSD1306_Device* DeviceHandle, const char* Text, int x, int y, bool Color ) {
    int Length = 0;
    int i = 0;
    int x2 = 0;
    int y2 = 0;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
    NullCheck( DeviceHandle->Font, return );
    NullCheck( DeviceHandle->Font->Data, return );
    NullCheck( Text, return );

    CheckBounds( x >= DeviceHandle->Width, return );
    CheckBounds( y >= DeviceHandle->Height, return );

    for ( i = 0, x2 = x, y2 = y, Length = strlen( Text ); i < Length; i++ ) {
        FontDrawChar( DeviceHandle, Text[ i ], x2, y2, Color );
        x2+= FontGetCharWidth( DeviceHandle->Font, Text[ i ] );
    }
}

void FontDrawCharUnaligned( struct SSD1306_Device* DeviceHandle, char c, int x, int y, bool Color ) {
    const uint8_t* FontOffset = NULL;
    struct FontDef* Font = NULL;
    int CharSizeInBytes = 0;
    int CharHeight = 0;
    int CharWidth = 0;
    int x2 = 0;
    int y2 = 0;
    int i = 0;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
    NullCheck( DeviceHandle->Font, return );
    NullCheck( DeviceHandle->Font->Data, return );

    Font = DeviceHandle->Font;
    CharWidth = FontGetCharWidth( Font, c );
    CharHeight = FontGetCharHeight( Font, c );
    
    CheckBounds( x >= ( DeviceHandle->Width ) - CharWidth, return );
    CheckBounds( y >= ( DeviceHandle->Height ) - CharHeight, return );

    if ( c < Font->StartChar || c > Font->EndChar ) {
        return;
    }

    /* Height is size in bytes, so a 16px high font would be 2 bytes tall */
    CharSizeInBytes = ( Font->Width * Font->Height ) + 1;
    FontOffset = &Font->Data[ ( ( c - Font->StartChar ) * CharSizeInBytes ) + 1 ];

    for ( x2 = 0; x2 < CharWidth; x2++ ) {
        for ( y2 = 0; y2 < Font->Height; y2++ ) {
            for ( i = 7; i >= 0; i-- ) {
                if ( *FontOffset & BIT( i ) ) {
                    SSD1306_DrawPixel( DeviceHandle,
                        x + x2,
                        y + ( i + ( y2 * 8 ) ),
                        Color
                    ); 
                }
            }

            FontOffset++;
        }
    }
}

void FontDrawStringUnaligned( struct SSD1306_Device* DeviceHandle, const char* Text, int x, int y, bool Color ) {
    int Length = 0;
    int i = 0;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
    NullCheck( DeviceHandle->Font, return );
    NullCheck( DeviceHandle->Font->Data, return );
    NullCheck( Text, return );

    CheckBounds( x >= DeviceHandle->Width, return );
    CheckBounds( y >= DeviceHandle->Height, return );

    for ( i = 0, Length = strlen( Text ); i < Length; i++ ) {
        FontDrawCharUnaligned( DeviceHandle, Text[ i ], x, y, Color );
        x+= FontGetCharWidth( DeviceHandle->Font, Text[ i ] );
    }
}

void FontDrawAnchoredString( struct SSD1306_Device* DeviceHandle, const char* Text, TextAnchor Anchor , bool Color ) {
    int StringLengthInPixels = 0;
    int CharHeight = 0;
    int MidpointX = 0;
    int MidpointY = 0;
    int x = 0;
    int y = 0;

    NullCheck( DeviceHandle, return );
    NullCheck( DeviceHandle->Framebuffer, return );
    NullCheck( DeviceHandle->Font, return );
    NullCheck( DeviceHandle->Font->Data, return );
    NullCheck( Text, return );    

    StringLengthInPixels = FontMeasureString( DeviceHandle->Font, Text );
    CharHeight = FontGetCharHeight( DeviceHandle->Font, ' ' );
    MidpointX = ( DeviceHandle->Width / 2 ) - 1;
    MidpointY = ( DeviceHandle->Height / 2 ) - 1;

    switch ( Anchor ) {
        case TextAnchor_North: {
            x = MidpointX - ( StringLengthInPixels / 2 );
            y = 0;
            break;
        }
        case TextAnchor_NorthEast: {
            x = DeviceHandle->Width - 1 - StringLengthInPixels;
            y = 0;
            break;
        }
        case TextAnchor_NorthWest: {
            x = 0;
            y = 0;
            break;
        }
        case TextAnchor_East: {
            y = MidpointY - ( CharHeight / 2 );
            x = DeviceHandle->Width - 1 - StringLengthInPixels;
            break;
        }
        case TextAnchor_West: {
            y = MidpointY - ( CharHeight / 2 );
            x = 0;
            break;
        }
        case TextAnchor_SouthEast: {
            y = DeviceHandle->Height - 1 - CharHeight;
            x = DeviceHandle->Width - 1 - StringLengthInPixels;
            break;
        }
        case TextAnchor_SouthWest: {
            y = DeviceHandle->Height - 1 - CharHeight;
            x = 0;
            break;
        }
        case TextAnchor_South: {
            x = MidpointX - ( StringLengthInPixels / 2 );
            y = DeviceHandle->Height - 1 - CharHeight;
            break;
        }
        case TextAnchor_Center: {
            x = MidpointX - ( StringLengthInPixels / 2 );
            y = MidpointY - ( CharHeight / 2 );
            break;
        }
        default: {
            return;
        }
    };
    
    FontDrawStringUnaligned( DeviceHandle, Text, x, y, Color );
}
