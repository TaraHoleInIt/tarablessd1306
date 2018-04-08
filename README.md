<!---
 Copyright (c) 2017 Tara Keeling
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# SSD1306 Component for the ESP32 and ESP-IDF SDK

## About:  
This is a simple component for the SSD1306 display.  
It supports multiple display sizes on both i2c and spi interfaces.  
  
## Initialization:  
SSD1306_Init_I2C: Sets up a display that will be connected via i2c  
SSD1306_Init_SPI: Sets up a display that will be connected via SPI  
SSD1306_HWReset: Performs a hardware reset, callback to handle reset pin must be provided  
SSD1306_I2CMasterInitDefault: If configured, will provide initialization for the i2c bus  
SSD1306_I2CMasterAttachDisplayDefault: If configured, connects a display via i2c using the component configuration  
  
## Drawing API:  
SSD1306_Update: Sends the local framebuffer to the display, this MUST be called if you want to see what you drew.  
SSD1306_Clear: Clears the local framebuffer (Note: Does not send cleared framebuffer to screen on it's own)  
SSD1306_DrawPixel: Draws a pixel at the given coordinates  
SSD1306_DrawHLine: Draws a horizontal line  
SSD1306_DrawVLine: Draws a vertical line  
SSD1306_DrawLine: Can draw a horizontal, vertical, or slanted line  
SSD1306_DrawBox: Draws a box with the given coordinates, can optionally fill with a colour  
  
## Colors:  
SSD_COLOR_BLACK: Black  
SSD_COLOR_WHITE: White  
SSD_COLOR_XOR: XOR's the pixel already at the given location  
  
## Text rendering:  
SSD1306_SetFont: Sets the current font on the given display  
SSD1306_FontForceProportional: Forces a monospace font to be rendered proportionally  
SSD1306_FontForceMonospace: Forces a proportional font to be rendered as monospace  
SSD1306_FontGetWidth: Returns the width of the current font, for proportional fonts this is the maximum width  
SSD1306_FontGetHeight: Returns the height of the current font  
SSD1306_FontGetCharWidth: Returns the width of the given character for the current font  
SSD1306_FontGetCharHeight: Returns the height of characters in the current font, no character parameter needed since they're all the same  
SSD1306_FontMeasureString: Returns the number of pixels wide the given string is with the current font  
SSD1306_FontDrawChar: Draws a character with the current font with the given coordinates  
SSD1306_FontDrawString: Draws a string with the current font with the given coordinates  
SSD1306_FontDrawAnchoredString: Draws a string aligned to one of the cardinal directions, for example TextAnchor_NorthWest is 0,0  
SSD1306_FontGetAnchoredStringCoords: Used to get x and y coordinates of the given string and text anchor  
  
## Text anchors:  
TextAnchor_NorthEast: String is drawn from the top right of the screen  
TextAnchor_NorthWest: String is drawn from the top left of the screen  
TextAnchor_North: String is drawn at the top center of the screen  
TextAnchor_South: String is drawn at the bottom center of the screen  
TextAnchor_SouthEast: String is drawn at the bottom right of the screen  
TextAnchor_SouthWest: String is drawn at the bottom left of the screen  
TextAnchor_Center: String is drawn at the center of the screen  
  
## Fonts:  
Droid sans fallback 11x12  
Droid sans fallback 16x17  
Droid sans fallback 25x25  
  
Droid sans mono 7x12  
Droid sans mono 13x21  
Droid sans mono 17x27  
  
Liberation mono 8x11  
Liberation mono 13x20  
Liberation mono 17x27  

