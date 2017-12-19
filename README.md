<!---
 Copyright (c) 2017 Tara Keeling
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# SSD1306 Component for the ESP32 and ESP-IDF SDK

###### Status:
- Supports multiple displays on both I2C and SPI interfaces (at the same time)
- Support for a larger virtual display which can be split up to several smaller ones
- Basic font rendering
- A few basic drawing operations

###### TODO/Issues:
- SSD1306_DrawLine Is incomplete and only does horizontal and vertical lines
- More drawing operations are needed, and the existing ones need more work
- Font rendering needs more work
- Virtual display could be done better

###### Notes:
- 128x32 And other displays other than 128x64 are only partially tested. Let me know what works.
- If you plan on hooking up more than 3 SPI displays you'll need to change NO_CS in spi_master.c in components/driver/ in your esp-idf SDK
- Control pins for CS and DC in SPI mode can be iffy, it took a bit of fiddling to find pins it was happy with
