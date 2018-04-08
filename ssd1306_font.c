#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ssd1306.h"
#include "ssd1306_draw.h"
#include "ssd1306_font.h"

__attribute__( ( always_inline ) ) static inline int RoundUpFontHeight( const struct SSD1306_FontDef* Font ) {
    int Height = Font->Height;

    if ( ( Height % 8 ) != 0 ) {
        return ( ( Height + 7 ) / 8 ) * 8;
    }

    return Height;
}

__attribute__( ( always_inline ) ) static inline const uint8_t* GetCharPtr( const struct SSD1306_FontDef* Font, char Character ) {
    return &Font->FontData[ ( Character - Font->StartChar ) * ( ( Font->Width * ( RoundUpFontHeight( Font ) / 8 ) ) + 1 ) ];
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
    int WidestChar = 1;
    int Width = 0;
    int i = 0;

    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    for ( i = Display->Font->StartChar; i <= Display->Font->EndChar; i++ ) {
        Width = SSD1306_FontGetCharWidth( Display, ( char ) i );
        WidestChar = ( Width > WidestChar ) ? Width : WidestChar;
    }

    return Display->Width / WidestChar;
}

int SSD1306_FontGetMaxCharsPerColumn( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Width / SSD1306_FontGetCharHeight( Display );    
}

int SSD1306_FontGetCharHeight( struct SSD1306_Device* Display ) {
    NullCheck( Display, return 0 );
    NullCheck( Display->Font, return 0 );

    return Display->Font->Height;
}

void SSD1306_FontDrawChar( struct SSD1306_Device* Display, char Character, int x, int y, int Color ) {
    const uint8_t* GlyphData = NULL;
    int Width = 0;
    int Height = 0;
    int Bit = 0;
    int x2 = 0;
    int y2 = 0;

    NullCheck( Display, return );
    NullCheck( Display->Font, return );

    GlyphData = GetCharPtr( Display->Font, Character ) + 1;

    if ( GlyphData != NULL ) {
        Width = SSD1306_FontGetCharWidth( Display, Character );
        Height = RoundUpFontHeight( Display->Font ) / 8;

        for ( x2 = 0; x2 < Width; x2++ ) {
            for ( y2 = 0; y2 < Height; y2++ ) {
                for ( Bit = 7; Bit >= 0; Bit-- ) {
                    if ( *GlyphData & BIT( Bit ) ) {
                        SSD1306_DrawPixel( Display,
                            x + x2,
                            y + Bit + ( y2 * 8 ),
                            Color
                        );
                    }
                }

                GlyphData++;
            }
        }
    }
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
