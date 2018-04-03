#ifndef _SSD1306_DRAW_H_
#define _SSD1306_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SSD_COLOR_WHITE 1
#define SSD_COLOR_BLACK 0

#define ClipBounds( x, min, max, retexpr ) { \
    if ( x < min ) { \
        ESP_LOGE( __FUNCTION__, "%s ClipBounds: %s < %s", __FUNCTION__, #x, #min ); \
        retexpr; \
    } \
    else if ( x > max ) { \
        ESP_LOGE( __FUNCTION__, "%s ClipBounds: %s > %s", __FUNCTION__, #x, #max ); \
        retexpr; \
    } \
}

void SSD1306_Clear( struct SSD1306_Device* DeviceHandle, int Color );
void SSD1306_DrawPixelFast( struct SSD1306_Device* DeviceHandle, int X, int Y, int Color );
void SSD1306_DrawPixel( struct SSD1306_Device* DeviceHandle, int X, int Y, int Color );
void SSD1306_DrawHLine( struct SSD1306_Device* DeviceHandle, int x, int y, int Width, int Color );
void SSD1306_DrawVLine( struct SSD1306_Device* DeviceHandle, int x, int y, int Height, int Color );
void SSD1306_DrawLine( struct SSD1306_Device* DeviceHandle, int x0, int y0, int x1, int y1, int Color );
void SSD1306_DrawRect( struct SSD1306_Device* DeviceHandle, int x, int y, int x2, int y2, int Color );

#endif
