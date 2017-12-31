#ifndef _SSD1306_FONT_H_
#define _SSD1306_FONT_H_

#include <stdbool.h>

struct FontDef {
    const uint8_t* Data;
    int Width;  /* Width in bytes */
    int Height; /* Height in bytes */
    int StartChar;  /* Which ascii # the font starts with (usually a space or !) */
    int EndChar;    /* Ending ascii # the font ends with */
};

typedef enum {
    TextAnchor_East = 0,
    TextAnchor_West,
    TextAnchor_North,
    TextAnchor_South,
    TextAnchor_NorthEast,
    TextAnchor_NorthWest,
    TextAnchor_SouthEast,
    TextAnchor_SouthWest,
    TextAnchor_Center
} TextAnchor;

extern struct FontDef Font_Ubuntu_Mono_6x10;
extern struct FontDef Font_Liberation_Sans_15x16;
extern struct FontDef Font_Liberation_Serif_19x19;
extern struct FontDef Font_Comic_Neue_25x28;

struct SSD1306_Device;

int SSD1306_FontGetCharHeight( struct FontDef* FontHandle, char c );
int SSD1306_FontMeasureString( struct FontDef* FontHandle, const char* Text );
int SSD1306_FontGetCharWidth( struct FontDef* FontHandle, char c );

void SSD1306_FontDrawChar( struct SSD1306_Device* DeviceHandle, char c, int x, int y, bool Color );
void SSD1306_FontDrawString( struct SSD1306_Device* DeviceHandle, const char* Text, int x, int y, bool Color );

void SSD1306_FontDrawCharUnaligned( struct SSD1306_Device* DeviceHandle, char c, int x, int y, bool Color );
void SSD1306_FontDrawStringUnaligned( struct SSD1306_Device* DeviceHandle, const char* Text, int x, int y, bool Color );
void SSD1306_FontDrawAnchoredString( struct SSD1306_Device* DeviceHandle, const char* Text, TextAnchor Anchor , bool Color );

#endif
