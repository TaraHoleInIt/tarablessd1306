#ifndef _SSD1306_FONT_H_
#define _SSD1306_FONT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SSD1306_Device;

/* 
 * X-GLCD Font format:
 *
 * First byte of glyph is it's width in pixels.
 * Each data byte represents 8 pixels going down from top to bottom.
 * 
 * Example glyph layout for a 16x16 font
 * 'a': [Glyph width][Pixel column 0][Pixel column 1] where the number of pixel columns is the font height divided by 8
 * 'b': [Glyph width][Pixel column 0][Pixel column 1]...
 * 'c': And so on...
 */

struct SSD1306_FontDef {
    const uint8_t* FontData;

    int Width;
    int Height;

    int StartChar;
    int EndChar;

    bool Monospace;
};

bool SSD1306_SetFont( struct SSD1306_Device* Display, const struct SSD1306_FontDef* Font );

void SSD1306_FontForceProportional( struct SSD1306_Device* Display, bool Force );
void SSD1306_FontForceMonospace( struct SSD1306_Device* Display, bool Force );

int SSD1306_FontGetWidth( struct SSD1306_Device* Display );
int SSD1306_FontGetHeight( struct SSD1306_Device* Display );

int SSD1306_FontGetMaxCharsPerRow( struct SSD1306_Device* Display );
int SSD1306_FontGetMaxCharsPerColumn( struct SSD1306_Device* Display );

int SSD1306_FontGetCharWidth( struct SSD1306_Device* Display, char Character );
int SSD1306_FontGetCharHeight( struct SSD1306_Device* Display );
int SSD1306_FontMeasureString( struct SSD1306_Device* Display, const char* Text );\

void SSD1306_FontDrawChar( struct SSD1306_Device* Display, char Character, int x, int y, int Color );
void SSD1306_FontDrawString( struct SSD1306_Device* Display, int x, int y, const char* Text, int Color );

extern const struct SSD1306_FontDef Font_consola_mono_8x16;
extern const struct SSD1306_FontDef Font_droid_sans_fallback_11x12;
extern const struct SSD1306_FontDef Font_droid_sans_mono_7x12;
extern const struct SSD1306_FontDef Font_go_mono_8x12;
extern const struct SSD1306_FontDef Font_liberation_mono_8x11;

#ifdef __cplusplus
}
#endif

#endif
