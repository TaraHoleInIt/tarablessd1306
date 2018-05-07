/**
 * Copyright (c) 2017-2018 Tara Keeling
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "ssd1306.h"
#include "ssd1306_draw.h"
#include "ssd1306_font.h"

static int RoundUpFontHeight( const struct SSD1306_FontDef* Font ) {
    int Height = Font->Height;

    if ( ( Height % 8 ) != 0 ) {
        return ( ( Height + 7 ) / 8 ) * 8;
    }

    return Height;
}

static const uint8_t* GetCharPtr( const struct SSD1306_FontDef* Font, char Character ) {
    return &Font->FontData[ ( Character - Font->StartChar ) * ( ( Font->Width * ( RoundUpFontHeight( Font ) / 8 ) ) + 1 ) ];
}

void SSD1306_FontDrawChar( struct SSD1306_Device* DisplayHandle, char Character, int x, int y, int Color ) {
    const uint8_t* GlyphData = NULL;
    int GlyphColumnLen = 0;
    int CharStartX =  0;
    int CharStartY = 0;
    int CharWidth = 0;
    int CharHeight = 0;
    int CharEndX = 0;
    int CharEndY = 0;
    int OffsetX = 0;
    int OffsetY = 0;
    int YByte = 0;
    int YBit = 0;
    int i = 0;

    NullCheck( DisplayHandle, return );
    NullCheck( DisplayHandle->Font, return );
    
    NullCheck( ( GlyphData = GetCharPtr( DisplayHandle->Font, Character ) ), return );

    if ( Character >= DisplayHandle->Font->StartChar || Character <= DisplayHandle->Font->EndChar ) {
        /* The first byte in the glyph data is the width of the character in pixels, skip over */
        GlyphData++;
        GlyphColumnLen = RoundUpFontHeight( DisplayHandle->Font ) / 8;
        
        CharWidth = SSD1306_FontGetCharWidth( DisplayHandle, Character );
        CharHeight = SSD1306_FontGetHeight( DisplayHandle );

        CharStartX = x;
        CharStartY = y;
        
        CharEndX = CharStartX + CharWidth;
        CharEndY = CharStartY + CharHeight;

        /* If the character is partially offscreen offset the end by
        * distance between (coord) and 0.
        */
        OffsetX = ( CharStartX < 0 ) ? abs( CharStartX ) : 0;
        OffsetY = ( CharStartY < 0 ) ? abs( CharStartY ) : 0;

        /* This skips into the proper column within the glyph data */
        GlyphData+= ( OffsetX * GlyphColumnLen );

        CharStartX+= OffsetX;
        CharStartY+= OffsetY;

        /* Do not attempt to draw if this character is entirely offscreen */
        if ( CharEndX < 0 || CharStartX >= DisplayHandle->Width || CharEndY < 0 || CharStartY >= DisplayHandle->Height ) {
            ClipDebug( x, y );
            return;
        }

        /* Do not attempt to draw past the end of the screen */
        CharEndX = ( CharEndX >= DisplayHandle->Width ) ? DisplayHandle->Width - 1 : CharEndX;
        CharEndY = ( CharEndY >= DisplayHandle->Height ) ? DisplayHandle->Height - 1 : CharEndY;

        for ( x = CharStartX; x < CharEndX; x++ ) {
            for ( y = CharStartY, i = 0; y < CharEndY && i < CharHeight; y++, i++ ) {
                YByte = ( i + OffsetY ) / 8;
                YBit = ( i + OffsetY ) & 0x07;

                if ( GlyphData[ YByte ] & BIT( YBit ) ) {
                    SSD1306_DrawPixel( DisplayHandle, x, y, Color );
                }            
            }

            GlyphData+= GlyphColumnLen;
        }
    }
}

bool SSD1306_SetFont( struct SSD1306_Device* Display, const struct SSD1306_FontDef* Font ) {
    NullCheck( Display, return false );
    NullCheck( Font, return false );

    Display->FontForceProportional = false;
    Display->FontForceMonospace = false;
    Display->Font = Font;

    return true;
}

void SSD1306_FontForceProportional( struct SSD1306_Device* Display, bool Force ) {
    NullCheck( Display, return );
    NullCheck( Display->Font, return );

    Display->FontForceProportional = Force;
}

void SSD1306_FontForceMonospace( struct SSD1306_Device* Display, bool Force ) {
    NullCheck( Display, return );
    NullCheck( Display->Font, return );

    Display->FontForceMonospace = Force;
}

int SSD1306_FontGetWidth( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Font->Width;
}

int SSD1306_FontGetHeight( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Font->Height;
}

int SSD1306_FontGetCharWidth( struct SSD1306_Device* Display, char Character ) {
    const uint8_t* CharPtr = NULL;
    int Width = 0;

    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );
    
    if ( Character >= Display->Font->StartChar && Character <= Display->Font->EndChar ) {
        CharPtr = GetCharPtr( Display->Font, Character );

        Width = ( Display->Font->Monospace == true ) ? Display->Font->Width : *CharPtr;

        if ( Display->FontForceMonospace == true ) {
            Width = Display->Font->Width;
        }

        if ( Display->FontForceProportional == true ) {
            Width = *CharPtr;
        }
    }

    return Width;
}

int SSD1306_FontGetMaxCharsPerRow( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Width / Display->Font->Width;
}

int SSD1306_FontGetMaxCharsPerColumn( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Height / Display->Font->Height;    
}

int SSD1306_FontGetCharHeight( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Font->Height;
}

int SSD1306_FontMeasureString( struct SSD1306_Device* Display, const char* Text ) {
    int Width = 0;
    int Len = 0;

    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );
    NullCheck( Text, return 0 );

    for ( Len = strlen( Text ); Len >= 0; Len--, Text++ ) {
        if ( *Text >= Display->Font->StartChar && *Text <= Display->Font->EndChar ) {
            Width+= SSD1306_FontGetCharWidth( Display, *Text );
        }
    }

    return Width;
}

void SSD1306_FontDrawString( struct SSD1306_Device* Display, int x, int y, const char* Text, int Color ) {
    int Len = 0;
    int i = 0;

    NullCheck( Display, return );
    NullCheck( Display->Font, return );
    NullCheck( Text, return );

    for ( Len = strlen( Text ), i = 0; i < Len; i++ ) {
        SSD1306_FontDrawChar( Display, *Text, x, y, Color );

        x+= SSD1306_FontGetCharWidth( Display, *Text );
        Text++;
    }
}

void SSD1306_FontDrawAnchoredString( struct SSD1306_Device* Display, TextAnchor Anchor, const char* Text, int Color ) {
    int x = 0;
    int y = 0;

    NullCheck( Display, return );
    NullCheck( Text, return );

    SSD1306_FontGetAnchoredStringCoords( Display, &x, &y, Anchor, Text );
    SSD1306_FontDrawString( Display, x, y, Text, Color );
}

void SSD1306_FontGetAnchoredStringCoords( struct SSD1306_Device* Display, int* OutX, int* OutY, TextAnchor Anchor, const char* Text ) {
    int StringWidth = 0;
    int StringHeight = 0;

    NullCheck( Display, return );
    NullCheck( OutX, return );
    NullCheck( OutY, return );
    NullCheck( Text, return );

    StringWidth = SSD1306_FontMeasureString( Display, Text );
    StringHeight = SSD1306_FontGetCharHeight( Display );

    switch ( Anchor ) {
        case TextAnchor_East: {
            *OutY = ( Display->Height / 2 ) - ( StringHeight / 2 );
            *OutX = ( Display->Width - StringWidth );

            break;
        }
        case TextAnchor_West: {
            *OutY = ( Display->Height / 2 ) - ( StringHeight / 2 );
            *OutX = 0;

            break;
        }
        case TextAnchor_North: {
            *OutX = ( Display->Width / 2 ) - ( StringWidth / 2 );
            *OutY = 0;

            break;
        }
        case TextAnchor_South: {
            *OutX = ( Display->Width / 2 ) - ( StringWidth / 2 );
            *OutY = ( Display->Height - StringHeight );
            
            break;
        }
        case TextAnchor_NorthEast: {
            *OutX = ( Display->Width - StringWidth );
            *OutY = 0;

            break;
        }
        case TextAnchor_NorthWest: {
            *OutY = 0;
            *OutX = 0;

            break;
        }
        case TextAnchor_SouthEast: {
            *OutY = ( Display->Height - StringHeight );
            *OutX = ( Display->Width - StringWidth );

            break;
        }
        case TextAnchor_SouthWest: {
            *OutY = ( Display->Height - StringHeight );
            *OutX = 0;

            break;
        }
        case TextAnchor_Center: {
            *OutY = ( Display->Height / 2 ) - ( StringHeight / 2 );
            *OutX = ( Display->Width / 2 ) - ( StringWidth / 2 );

            break;
        }
        default: {
            *OutX = 128;
            *OutY = 64;
            
            break;
        }
    };
}
